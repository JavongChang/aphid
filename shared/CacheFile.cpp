#include "CacheFile.h"
#include <AllHdf.h>
#include <HBase.h>

CacheFile::CacheFile() : HFile() {}
CacheFile::CacheFile(const char * name) : HFile(name) {}

bool CacheFile::create(const std::string & fileName)
{
	if(!HObject::FileIO.open(fileName.c_str(), HDocument::oCreate)) {
		setLatestError(BaseFile::FileNotReadable);
		return false;
	}
	
	setDocument(HObject::FileIO);

	return BaseFile::create(fileName);
}

bool CacheFile::open(const std::string & fileName)
{
    if(!FileExists(fileName)) {
		setLatestError(BaseFile::FileNotFound);
		return false;
	}
	
	if(!HObject::FileIO.open(fileName.c_str(), HDocument::oReadAndWrite)) {
		setLatestError(BaseFile::FileNotReadable);
		return false;
	}
	
	setDocument(HObject::FileIO);

	return BaseFile::open(fileName);
}

bool CacheFile::save()
{
    return true;
}

bool CacheFile::close()
{
	useDocument();
	std::map<std::string, HBase *>::iterator it;
	for(it = m_entries.begin(); it != m_entries.end(); ++it) {
	    if((*it).second) {
	        (*it).second->close();
	        delete (*it).second;
	    }
	}
		
	return HFile::close();
}

HBase * CacheFile::getNamedEntry(const std::string & name)
{
    std::map<std::string, HBase *>::iterator it = m_entries.find(name);
    if(it == m_entries.end()) return 0;
    return (*it).second;
}

void CacheFile::openEntry(const std::string & name)
{
	HBase * entry = new HBase(name);
	m_entries[name] = entry;
}

void CacheFile::closeEntry(const std::string & name)
{
    HBase * p = getNamedEntry(name);
    if(p == 0) return;
	p->close();
	delete p;
	m_entries[name] = 0;
}

bool CacheFile::openSlice(const std::string & entryName, const std::string & sliceName)
{
	HBase * g = getNamedEntry(entryName);
	if(g == 0) return false;
	
	if(!g->hasNamedData(sliceName.c_str()))
	    g->addVector3Data(sliceName.c_str(), 1024);
	
	VerticesHDataset * pset = new VerticesHDataset(sliceName.c_str());
	pset->open(g->fObjectId);
	
	m_slices[HObject::FullPath(entryName, sliceName)] = pset;
	
	return true;
}

void CacheFile::closeSlice(const std::string & entryName, const std::string & sliceName)
{
	const std::string slicePath = HObject::FullPath(entryName, sliceName);
	
	if(m_slices.find(slicePath) == m_slices.end()) return;
	
	m_slices[slicePath]->close();
}

void CacheFile::saveEntrySize(const std::string & entryName, unsigned size)
{
    HBase * g = getNamedEntry(entryName);
	if(g == 0) return;
	
	if(!g->hasNamedAttr(".size"))
	    g->addIntAttr(".size");
	
	int x = size;
	g->writeIntAttr(".size", &x);
}

void CacheFile::writeSliceVector3(const std::string & entryName, const std::string & sliceName, unsigned start, unsigned count, Vector3F * data)
{
	const std::string slicePath = HObject::FullPath(entryName, sliceName);
	if(m_slices.find(slicePath) == m_slices.end()) return;
	
	VerticesHDataset * p = (VerticesHDataset *)m_slices[slicePath];
	p->setNumVertices(start + count);
	
	if(!p->hasEnoughSpace()) p->resize();
	
	HDataset::SelectPart pt;
	pt.start[0] = start * 3;
	pt.count[0] = 1;
	pt.block[0] = count * 3;

	p->write((char *)data, &pt);
}

void CacheFile::readSliceVector3(const std::string & entryName, const std::string & sliceName, unsigned start, unsigned count, Vector3F * data)
{
	const std::string slicePath = HObject::FullPath(entryName, sliceName);
	if(m_slices.find(slicePath) == m_slices.end()) return;
	
	VerticesHDataset * p = (VerticesHDataset *)m_slices[slicePath];
	
	unsigned maxcount = isCached(entryName, sliceName);
	
	if(maxcount < start) return;
	
	p->setNumVertices(maxcount);
	
	unsigned c = count;
	if(c > maxcount - start) c = maxcount - start;
	
	HDataset::SelectPart pt;
	pt.start[0] = start * 3;
	pt.count[0] = 1;
	pt.block[0] = c * 3;
	
	p->read((char *)data, &pt);
}

void CacheFile::setCached(const std::string & entryName, const std::string & sliceName, unsigned size)
{
	m_cachedSlices[HObject::FullPath(entryName, sliceName)] = size;
}

unsigned CacheFile::isCached(const std::string & entryName, const std::string & sliceName)
{
	const std::string slicePath = HObject::FullPath(entryName, sliceName);
	if(m_cachedSlices.find(slicePath) == m_cachedSlices.end()) return 0;
	return m_cachedSlices[slicePath];
}

void CacheFile::clearCached()
{
	m_cachedSlices.clear();
}