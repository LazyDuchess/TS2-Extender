#include "Addresses.h"
#include "ts2/nRZSceneGraph.h"

cMaterialParser* nRZSceneGraph::MaterialParser() {
	return ((cMaterialParser * (__stdcall*)())Addresses::GetMaterialParser)();
}