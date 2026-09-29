#include "ts2/nRZSceneGraph"

cMaterialParser* nRZSceneGraph::MaterialParser() {
	return ((cMaterialParser * (__stdcall*)())Addresses::GetMaterialParser)();
}