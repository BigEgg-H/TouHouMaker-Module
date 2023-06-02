#ifndef TIGAS_H
#define TIGAS_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/io/resource_loader.h"
#include "core/object/ref_counted.h"
#include "scene/3d/node_3d.h"
#include "scene/resources/packed_scene.h"
#include "core/templates/list.h"
#include "modules/gdscript/gdscript.h"

#define GASError String
#define GAS_ERR_OK ""

class Mod {
public:
	virtual GASError LoadInstance(const String &Path) = 0;
	virtual GASError ClearAll() = 0;
};

class MapMod : public Mod {
public:
	GASError LoadInstance(const String &Path);
	GASError ClearAll();
	Node *GetMod(int Index);

public:
	List<Node *> ModPool;
};

class BulletMod : public Mod {
public:
	GASError LoadInstance(const String &Path);
	GASError ClearAll();
	ScriptInstance *GetMod(int Index);

public:
	List<ScriptInstance*> ModPool;
};

/// <summary>
///	TIGAS
/// 模板-实例化泛型分析系统
/// </summary>
class TIGAS{
public:
	static GASError LoadModPack(const String &Path);
	static GASError LoadMapMod(const String &Path);
	static GASError LoadBulletMod(const String &Path);
	static GASError ClearModAll();
	static GASError ClearMapModAll();
	static GASError ClearBulletModAll();
	static void Kill();

public:
	static Node *GetMapMod(int ModID);
	static ScriptInstance *GetBulletMod(int BulletID);
	static BulletMod *GetBulletModTemplate();

};

#endif // !TIGAS_H
