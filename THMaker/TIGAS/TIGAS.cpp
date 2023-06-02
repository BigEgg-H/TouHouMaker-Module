#include "TIGAS.h"

//
// Mod Singleton Table
//

MapMod* MapModSingleton = memnew(MapMod);
BulletMod *BulletModSingleton = memnew(BulletMod);

//
// End
//

//
//TIGAS
//
GASError TIGAS::LoadModPack(const String &Path) {
	return "The feature not yet supported: Module packages";
}

GASError TIGAS::LoadMapMod(const String &Path) {
	return MapModSingleton->LoadInstance(Path);
}

GASError TIGAS::LoadBulletMod(const String &Path) {
	return BulletModSingleton->LoadInstance(Path);
}

GASError TIGAS::ClearModAll() {
	GASError err1 = BulletModSingleton->ClearAll();
	GASError err2 = MapModSingleton->ClearAll();
	if (err1 != GAS_ERR_OK) {
		return err1;
	} else {
		if (err2 != GAS_ERR_OK) {
			return err2;
		}
	}
	return GAS_ERR_OK;
}

GASError TIGAS::ClearMapModAll() {
	return MapModSingleton->ClearAll();
}

GASError TIGAS::ClearBulletModAll() {
	return BulletModSingleton->ClearAll();
}

void TIGAS::Kill() {
	ClearModAll();
	memdelete(MapModSingleton);
	memdelete(BulletModSingleton);
}

Node* TIGAS::GetMapMod(int ModID) {
	return MapModSingleton->GetMod(ModID);
}

ScriptInstance *TIGAS::GetBulletMod(int BulletID) {
	return BulletModSingleton->GetMod(BulletID);
}

BulletMod *TIGAS::GetBulletModTemplate() {
	return BulletModSingleton;
}

//
//MapMod
//
GASError MapMod::LoadInstance(const String &Path) {
	Ref<PackedScene> ModPS = ResourceLoader::load(Path);
	if (ModPS.is_valid()) {
		ModPool.push_back(ModPS.ptr()->instantiate());
		ModPS.unref();
		return GAS_ERR_OK;
	}
	return "The Map mod that loaded just now is not valid!";
}

GASError MapMod::ClearAll() {
	for (int i = 0; i < this->ModPool.size(); i++) {
		if (this->ModPool[i]->is_inside_tree()) {
			this->ModPool[i]->queue_free();
		}
		//memdelete(this->ModPool[i]);
	}
	this->ModPool.clear();
	return GAS_ERR_OK;
}

Node *MapMod::GetMod(int Index) {
	if (this->ModPool.size() <= Index) {
		return nullptr;
	}
	return this->ModPool[Index];
}

//
//BulletMod
//
GASError BulletMod::LoadInstance(const String &Path) {
	Ref<GDScript> buffer = ResourceLoader::load(Path);
	if (!buffer.is_valid()) {
		return "The bullet mod that loaded just now is not valid!";
	}
	Object *bullet = memnew(Object);
	ModPool.push_back(buffer.ptr()->instance_create(bullet));
	buffer.unref();
	return GAS_ERR_OK;
}

GASError BulletMod::ClearAll() {
	for (int i = 0; i < ModPool.size(); i++) {
		memdelete(ModPool[i]->get_owner());
	}
	this->ModPool.clear();
	return GAS_ERR_OK;
}

ScriptInstance *BulletMod::GetMod(int Index) {
	if (this->ModPool.size() <= Index) {
		return nullptr;
	}
	return this->ModPool[Index];
}
