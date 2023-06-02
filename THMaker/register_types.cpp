/* register_types.cpp */

#include "register_types.h"

#include "core/object/class_db.h"
#include "core/config/engine.h"

#include "HDVDBPS/HDVDBPS.h"
#include "GGSCCS/GGSCCS.h"

void initialize_THMaker_module(ModuleInitializationLevel p_level) {
	if (p_level == ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SERVERS) {
		//Registe class
		ClassDB::register_class<SCAnalyser>();
		ClassDB::register_class<GGSCCS>();
		//init singleton
		GGSCCSSSingleton = memnew(GGSCCS);
		Engine::get_singleton()->add_singleton(Engine::Singleton("GGSCCS", GGSCCSSSingleton));
	}
}

void uninitialize_THMaker_module(ModuleInitializationLevel p_level) {
	if (p_level == ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SERVERS) {
		GGSCCSSSingleton->Kill();
		memdelete(GGSCCSSSingleton);
	}
}
