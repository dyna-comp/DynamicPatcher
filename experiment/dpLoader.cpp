﻿// created by i-saint
// distributed under Creative Commons Attribution (CC BY) license.
// https://github.com/i-saint/DynamicObjLoader

#include "DynamicPatcher.h"
#include <algorithm>


dpLoader::dpLoader()
{
}

dpLoader::~dpLoader()
{
}

void      dpLoader::release()
{

}

dpBinary* dpLoader::loadBinary(const char *path)
{
    size_t len = strlen(path);
    if(len < 4) { return nullptr; }

    dpBinary *ret = nullptr;
    if(strcmp(&path[len-4], ".obj")==0) {
        ret = new dpObjFile();
    }
    else if(strcmp(&path[len-4], ".lib")==0) {
        ret = new dpLibFile();
    }
    else if(strcmp(&path[len-4], ".dll")==0 || strcmp(&path[len-4], ".exe")==0) {
        ret = new dpDllFile();
    }

    if(ret) {
        if(ret->loadFile(path) && ret->link(this)) {
            m_binaries.push_back(ret);
        }
        else {
            ret->release();
            ret = nullptr;
        }
    }
    return ret;
}

void* dpLoader::findHostSymbol(const char *name)
{
    // todo
    return nullptr;
}

void* dpLoader::findLoadedSymbol(const char *name)
{
    void *ret = nullptr;
    eachBinaries([&](dpBinary *bin){
        if(ret) { return; }
        if(void *s = bin->getSymbolTable().findSymbol(name)) {
            ret = s;
        }
    });
    return ret;
}

size_t dpLoader::getNumBinaries() const
{
    return m_binaries.size();
}

dpBinary* dpLoader::getBinary(size_t i)
{
    return m_binaries[i];
}

dpBinary* dpLoader::findBinary(const char *name)
{
    auto p = std::find_if(m_binaries.begin(), m_binaries.end(),
        [=](dpBinary *b){ return _stricmp(b->getPath(), name)==0; }
    );
    return p!=m_binaries.end() ? *p : nullptr;
}


dpCLinkage dpAPI dpLoader* dpCreateLoader()
{
    return new dpLoader();
}