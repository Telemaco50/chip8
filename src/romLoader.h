#ifndef __ROMLOADER_H__
#define __ROMLOADER_H__

#include <iostream>
#include <cstdint>

#include "logger.h"

class TRomLoader
{

    std::shared_ptr<TLogger> m_logger;
public:
    TRomLoader();
    ~TRomLoader();

    void loadRom(std::string& file_path, uint8_t* mem);
};

#endif