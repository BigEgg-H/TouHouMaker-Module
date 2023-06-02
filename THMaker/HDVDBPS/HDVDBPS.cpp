#include "modules/THMaker/TIGAS/TIGAS.h"
#include "HDVDBPS.h"

Thread *HDLoop;
Thread *HDLoop_second;
Thread *VDLoop;
Thread *VDLoop_second;
Thread *HSQCA;

Mutex *Team1Data;
Vector<BulletElement *> BulletPool_Team1;
Mutex *Team2Data;
Vector<BulletElement *> BulletPool_Team2;
Mutex *Team3Data;
Vector<BulletElement *> BulletPool_Team3;  

BPSMode Mode;
bool Dead;

//
//LoopFunctions
//
void HDLoopFunc(void *userData) {
	const DirectX::XMVECTOR LifeAdder = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	while (!Dead) {
		float BeginTime = clock() / 1000.0f; 

	}
}

void HDLoop_secondFunc(void *userData) {
	const DirectX::XMVECTOR LifeAdder = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	while (!Dead) {
	}
}

void VDLoopFunc(void *userData) {
	while (!Dead) {
	}
}

void VDLoop_secondFunc(void *userData) {
	while (!Dead) {
	}
}

void HSQCAFunc(void *userData) {
	BulletMod *ModTemplate = TIGAS::GetBulletModTemplate();
	while (!Dead) {
		//
		//Team1
		//
		Team1Data->lock();
		int bmTeam1 = BulletPool_Team1.size();
		Team1Data->unlock();
		for (int i = 0; i < bmTeam1; i++) {
			Team1Data->lock();
			if (i >= BulletPool_Team1.size()) {
				Team1Data->unlock();
				break;
			}
			BulletElement *pBulletBuffer = BulletPool_Team1[i];
			Team1Data->unlock();
			switch (pBulletBuffer->OperationCode) {
				case 1:
					pBulletBuffer->OffsetPos = (Vector2)ModTemplate->ModPool[pBulletBuffer->ModID]->call("BPSTick", pBulletBuffer->HDPos, pBulletBuffer->LifeTime, 1);
					break;
				case 2:
					pBulletBuffer->OperationCode = (int)ModTemplate->ModPool[pBulletBuffer->ModID]->call("BPSHit");
					break;
				case -1:
					Team1Data->lock();
					BulletPool_Team1.erase(pBulletBuffer);
					Team1Data->unlock();
					break;
				case 0:
					break;
				case -2:
					print_error(String("THM(HDVDBPS):One bullet error threw!From bullet mod: ") + pBulletBuffer->ModID);
					pBulletBuffer->OperationCode = -1;
					break;
			}
		}
		//
		//Team2
		//
		Team2Data->lock();
		int bmTeam2 = BulletPool_Team2.size();
		Team2Data->unlock();
		for (int i = 0; i < bmTeam2; i++) {
			Team2Data->lock();
			if (i >= BulletPool_Team2.size()) {
				Team2Data->unlock();
				break;
			}
			BulletElement *pBulletBuffer = BulletPool_Team2[i];
			Team2Data->unlock();
			switch (pBulletBuffer->OperationCode) {
				case 1:
					pBulletBuffer->OffsetPos = (Vector2)ModTemplate->ModPool[pBulletBuffer->ModID]->call("BPSTick", pBulletBuffer->HDPos, pBulletBuffer->LifeTime, 2);
					break;
				case 2:
					pBulletBuffer->OperationCode = (int)ModTemplate->ModPool[pBulletBuffer->ModID]->call("BPSHit");
					break;
				case -1:
					Team2Data->lock();
					BulletPool_Team2.erase(pBulletBuffer);
					Team2Data->unlock();
					break;
				case 0:
					break;
				case -2:
					print_error(String("THM(HDVDBPS):One bullet error threw!From bullet mod: ") + pBulletBuffer->ModID);
					pBulletBuffer->OperationCode = -1;
					break;
			}
		}
		//
		//Team3
		//
		Team3Data->lock();
		int bmTeam3 = BulletPool_Team3.size();
		Team3Data->unlock();
		for (int i = 0; i < bmTeam3; i++) {
			Team3Data->lock();
			if (i >= BulletPool_Team3.size()) {
				Team3Data->unlock();
				break;
			}
			BulletElement *pBulletBuffer = BulletPool_Team3[i];
			Team3Data->unlock();
			switch (pBulletBuffer->OperationCode) {
				case 1:
					pBulletBuffer->OffsetPos = (Vector2)ModTemplate->ModPool[pBulletBuffer->ModID]->call("BPSTick", pBulletBuffer->HDPos, pBulletBuffer->LifeTime, 3);
					break;
				case 2:
					pBulletBuffer->OperationCode = (int)ModTemplate->ModPool[pBulletBuffer->ModID]->call("BPSHit");
					break;
				case -1:
					Team3Data->lock();
					BulletPool_Team3.erase(pBulletBuffer);
					Team3Data->unlock();
					break;
				case 0:
					break;
				case -2:
					print_error(String("THM(HDVDBPS):One bullet error threw!From bullet mod: ") + pBulletBuffer->ModID);
					pBulletBuffer->OperationCode = -1;
					break;
			}
		}
	}
}

//
//HDVDBPS
//
BPSError HDVDBPS::StartSystem(BPSMode StartMode) {
	Mode = StartMode;
	Dead = false;
	Team1Data = memnew(Mutex);
	Team2Data = memnew(Mutex);
	Team3Data = memnew(Mutex);
	switch (StartMode) {
		case BPSMode::Balance:
			HDLoop = memnew(Thread);
			HDLoop_second = memnew(Thread);
			VDLoop = memnew(Thread);
			VDLoop_second = memnew(Thread);
			HSQCA = memnew(Thread);
			HDLoop->start(HDLoopFunc, nullptr);
			HDLoop_second->start(HDLoop_secondFunc, nullptr);
			VDLoop->start(VDLoopFunc, nullptr);
			VDLoop_second->start(VDLoop_secondFunc, nullptr);
			HSQCA->start(HSQCAFunc, nullptr);
			return BPS_ERR_OK;
		case BPSMode::Performance:
			return "Performance mode not yet supported";
		case BPSMode::Quality:
			return "Quality mode not yet supported";
	}
	return "Start mode not found!";
}

void HDVDBPS::Kill() {
	Dead = true;
	HDLoop->wait_to_finish();
	HDLoop_second->wait_to_finish();
	VDLoop->wait_to_finish();
	VDLoop_second->wait_to_finish();
	HSQCA->wait_to_finish();
	ClearAllBullet();
	memdelete(HDLoop);
	memdelete(HDLoop_second);
	memdelete(VDLoop);
	memdelete(VDLoop_second);
	memdelete(HSQCA);
	memdelete(Team1Data);
	memdelete(Team2Data);
	memdelete(Team3Data);
}

void HDVDBPS::SpawnBullet(int Mod, Vector2 Pos, Vector2 Dir, int TeamID) {
	BulletElement *bb = memnew(BulletElement);
	bb->ModID = Mod;
	bb->OperationCode = 1;
	bb->BulletInfo = DirectX::XMVectorSet(Pos.x, Pos.y, 0.0f, 0.0f);
	switch (TeamID) {
		case 1:
			Team1Data->lock();
			bb->SpawnTime = clock() / 1000.0f;
			BulletPool_Team1.append(bb);
			Team1Data->unlock();
			return ;
		case 2:
			Team2Data->lock();
			bb->SpawnTime = clock() / 1000.0f;
			BulletPool_Team2.append(bb);
			Team2Data->unlock();
			return ;
		case 3:
			Team3Data->lock();
			bb->SpawnTime = clock() / 1000.0f;
			BulletPool_Team3.append(bb);
			Team3Data->unlock();
			return ;
	}
	return ;
}

void HDVDBPS::ClearAllBullet() {
	Team1Data->lock();
	BulletPool_Team1.clear();
	Team1Data->unlock();
	Team2Data->lock();
	BulletPool_Team2.clear();
	Team2Data->unlock();
	Team3Data->lock();
	BulletPool_Team3.clear();
	Team3Data->unlock();
}
