#include "globalPosSolver.h"

void globalPosSolver::setNumCam(int numCam) {
	this->numCam = numCam;
}

void globalPosSolver::setNumId(int numId) {
	this->numId = numId;
}

void globalPosSolver::addLocalObjList(obj* objList) {
	localObjList[NextCamIdx++] = objList;
	//cout << "added list 1st node id : " <<localObjList[NextCamIdx - 1]->id << " position : " <<localObjList[NextCamIdx-1]->pos<<endl;
}

void globalPosSolver::process() {
	int count;
	obj* globalObj;
	obj* foundObj;
	Point2f sum(0, 0);
	Point2f globalPos(0, 0);

	for (int i = 0; i <numId; i++) {
		sum.x = 0.0;
		sum.y = 0.0;
		count = 0;
		globalObj = NULL;

		for (int j = 0; j < numCam; j++) {
			foundObj = findObj(localObjList[j], i);
			if (foundObj != NULL){
				if (count == 0) {
					globalObj = new obj;
				}
				sum.x = sum.x + foundObj->pos.x;
				sum.y = sum.y + foundObj->pos.y;
				count++;

				globalObj->camIdx = globalObj->camIdx + pow(10, j);
			}
		}

		if (count != 0) {
			globalPos.x = sum.x / count;
			globalPos.y = sum.y / count;

			globalObj->pos = globalPos;
			globalObj->id = i;
			globalObj->next = NULL;

			addGlobalObj(globalObj);
			globalObj = NULL;
		}
	}
}

obj* globalPosSolver::getGlocalObjList() {
	obj* ptr = globalObjList;
	cout << endl;
	cout << "getGlocalObjList" << endl;
	while (ptr != NULL) {
		cout<< "id : " << ptr->id << " whole cam # : " << ptr->camIdx <<" pos : " << ptr->pos << endl;
		ptr = ptr->next;
	}
	cout << endl;

	return globalObjList;
}

obj* globalPosSolver::findObj(obj* objList, int id) {
	obj* objPtr = objList;
	while (objPtr != NULL) {
		if (objPtr->id == id) {
			return objPtr;
		}
		objPtr = objPtr->next;
	}

	return objPtr;
}

void globalPosSolver::addGlobalObj(obj* newGolobalObjNode) {
	obj* objPtr = globalObjList;

	//if nothing in list, add to head
	if (objPtr == NULL) {
		globalObjList = newGolobalObjNode;
		return;
	}

	//add node to tail
	while (objPtr->next != NULL) {
		objPtr = objPtr->next;
	}
	objPtr->next = newGolobalObjNode;

	return;
}