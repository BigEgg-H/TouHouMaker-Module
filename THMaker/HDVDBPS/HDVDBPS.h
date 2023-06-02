#ifndef HDVD_BPS_H
#define HDVD_BPS_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/os/thread.h"
#include "servers/rendering_server.h"
#include "core/templates/list.h"

#include "../THMHelper.h"
#include "HDVDBPS_Data.h"
#include "HDVDBPS_Classes.h"

#define _XM_SSE4_INTRINSICS_
#include <DirectXMath.h>

#include <ctime>

#define BPSError String
#define BPS_ERR_OK ""

enum class BPSMode {
	Balance = 0,
	Performance,
	Quality
};

struct BulletElement {
	int ModID;
	int OperationCode;
	Vector2 OffsetPos;
	Vector2 HDPos;
	float SpawnTime;
	float LifeTime;
	/// <summary>
	/// x,y:position
	/// </summary>
	DirectX::XMVECTOR BulletInfo;

	BulletElement() {
		ModID = 0;
		OperationCode = 1;
	}

	BulletElement(BulletElement &_Father) {
		ModID = _Father.ModID;
		OperationCode = _Father.OperationCode;
		OffsetPos = _Father.OffsetPos;
		HDPos = _Father.HDPos;
		SpawnTime = _Father.SpawnTime;
		LifeTime = _Father.LifeTime;
		BulletInfo = _Father.BulletInfo;
	}
};

class HDVDBPS {
public:
	static BPSError StartSystem(BPSMode StartMode);
	static void Kill();
	static void SpawnBullet(int Mod, Vector2 Pos, Vector2 Dir, int TeamID);
	static void ClearAllBullet();
};

#endif
