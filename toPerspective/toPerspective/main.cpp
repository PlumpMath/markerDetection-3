#include "toPerspec.h"
#include "objStruct.h"
#include "globalPosSolver.h"
#include "aruco_detector.h"

#define camNumber 2

void showImageResult(Mat& glbIm, obj* objList);
void addObjToList(obj** head, obj* newNode);


int main(int argc, char** argv) {
	float width = 500;
	float height = 500;

	Mat srcIm[2], dstIm[2];
	Mat gloabalIm = Mat::ones(width, height, CV_32FC1);

	VideoCapture cam[2];
	toPerspec per[2];

	for (int i = 0; i < 2; i++) {
		cam[i].open(i);
	}

	if (cam[0].grab() && cam[1].grab()) {
		for (int i = 0; i < 2; i++) {
			cam[i].retrieve(srcIm[i]);
		}

		vector<Point2f> dst[2];

		for (int i = 0; i < 2; i++) {
			dst[i].push_back(Point2f(0, 0));
			dst[i].push_back(Point2f(width - 1, 0));
			dst[i].push_back(Point2f(width - 1, height - 1));
			dst[i].push_back(Point2f(0, height - 1));

			per[i].setInputImg(srcIm[i]);
			per[i].setBfHomoByClick();
			per[i].setAfHomo(dst[i]);
			per[i].setSize(width, height);

			per[i].Process();
		}
	}


	while (cam[0].grab() && cam[1].grab()) {
		for (int i = 0; i < camNumber; i++) {
			cam[i].retrieve(srcIm[i]);

			per[i].setInputImg(srcIm[i]);
			per[i].Process();
			dstIm[i] = per[i].getOutputImg();
		}

		//dstIm --------process(detection)--------> object id&position

		//aruco_detector START
		////////////////////////////////////////
		//aruco_detector* ptrArcDetector;
		vector<int>* markerIds[camNumber];
		vector<Point2f>* positions[camNumber];
		aruco_detector arcDetector[camNumber];

		for (int i = 0; i < camNumber; i++) {
			arcDetector[i].aruco_detector_initialize(i, 0, false, &dstIm[i]);
			arcDetector[i].process();
			markerIds[i] = arcDetector[i].get_ids();
			positions[i] = arcDetector[i].get_poses();
		}

		obj** cHead = new obj*[camNumber];

		for (int i = 0; i < camNumber; i++) {
			cHead[i] = NULL;
		}

		for (int i = 0; i < camNumber; i++) {
			for (int j = 0; j < (*markerIds[i]).size(); j++) {
				obj* ptrNewObj = new obj;
				ptrNewObj->camIdx = i;
				ptrNewObj->id = (*markerIds[i])[j];
				ptrNewObj->next = NULL;
				ptrNewObj->pos = (*positions[i])[j];

				obj* ptr = cHead[i];
				cout << endl;
				while (ptr != NULL) {
					cout << "id : " << ptr->id << "		cam # : " << ptr->camIdx << "	 pos : " << ptr->pos << endl;
					ptr = ptr->next;
				}
				cout << endl;
				ptr = NULL;

				addObjToList(&(cHead[i]), ptrNewObj);

				//obj* ptrNewObj;
				//obj newObj;
				//newObj.camIdx = i;
				//newObj.id = (*markerIds[i])[j];
				//newObj.next = NULL;
				//newObj.pos = (*positions[i])[j];

				//addObjToList(cHead[i], newObj);
				//cout << cHead[i]->id << endl;

				ptr = cHead[i];
				cout << endl;
				cout << "cHead[i] list" << "	id : " << cHead[i]->id << "		cam # : " << cHead[i]->camIdx << "	 pos : " << cHead[i]->pos << endl;
				while (ptr != NULL) {
					cout << "id : " << ptr->id << "		cam # : " << ptr->camIdx << "	pos : " << ptr->pos << endl;
					ptr = ptr->next;
				}
				cout << endl;
			}
		}
		////////////////////////////////////////
		//aruco_detector END

		//decide object global pose START
		/////////////////////////////////////////
		globalPosSolver glbPosSolver(2, 2);
		globalPosSolver* ptrGlbPosSolver = &glbPosSolver;
		
		for (int i = 0; i < camNumber; i++) {
			ptrGlbPosSolver->addLocalObjList(cHead[i]);
		}
		ptrGlbPosSolver -> process();
		obj* result = ptrGlbPosSolver -> getGlocalObjList();
		showImageResult(gloabalIm, result);
		//decide object global pose END
		////////////////////////////////////

		imshow("globalIm", gloabalIm);
		imshow("0", dstIm[0]);
		imshow("1", dstIm[1]);
		waitKey(0);

		
		for (int i = 0; i < camNumber; i++) {
			obj* p;
			obj* pNext;

			while (cHead[i]) {
				if (cHead[i]->next == NULL) {
					delete cHead[i];
					cHead[i] = NULL;
					break;
				}

				p = cHead[i];
				pNext = p->next;
				while (pNext->next) {
					p = pNext;
					pNext = pNext->next;
				}

				p->next = NULL;
				delete pNext;
			}
		}

		delete[] cHead;
		cHead = NULL;

		return 0;
	}

	return 0;
}

void showImageResult(Mat& glbIm, obj* objList) {
	int id=-1, allCam=-1;
	Point2f pos(-1,-1);
	Mat targeIm = glbIm;

	obj* p = objList;
	while (p) {
		id = p->id;
		allCam = p->camIdx;
		pos = p->pos;

		circle(targeIm, pos, 3, Scalar(0, 0, 255), CV_FILLED);
		String idString = to_string(id);
		String allCamString = to_string(allCam);
		String text = "id : " + idString + " cam : " + allCamString;
		putText(targeIm, text, pos, 1, 1, Scalar(0, 0, 255), 1, 8);

		p = p->next;
	}
}

void addObjToList(obj** head, obj* newNode) {
	obj* objPtr = *head;

	//if nothing in list, add to head
	if (objPtr == NULL) {
		*head = newNode;
		return;
	}

	//add node to tail
	while (objPtr->next != NULL) {
		objPtr = objPtr->next;
	}
	objPtr->next = newNode;

	return;
}

