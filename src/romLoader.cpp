#include "romLoader.h"
#include <string>
TRomLoader::TRomLoader()
{
    m_logger=TLogger::getInstance();
}

TRomLoader:: ~TRomLoader()
{


}

void TRomLoader::loadRom(std::string& file_path, uint8_t* mem){

    m_logger->log("Cargando ROM: " + file_path,ELogLevel::INFO);

    // Abrimos  el fichero en binario 
    std::ifstream file(file_path, std::ios::binary);

    //Comprobamos que se puede abrir sin problema

    if (!file)
    {
        m_logger->log("No se puede abrir esta vaina " +file_path, ELogLevel::ERROR);
        exit(1);

    }

    //Checkeamos el ramaño del rom
    file.seekg(0, std::ios::end);
    std::streamsize size= file.tellg();
    file.seekg(0, std::ios::beg);

    //Meter el fichero binario en el buffer
    if(file.read(reinterpret_cast<char*>(mem),size))
        m_logger->log("Fichero leido correctamente, el tamaño del monstruito es de " + std::to_string(size) +"bytes." ,ELogLevel::INFO);
    else
    {
        m_logger->log("Esto es ilegible illo",ELogLevel::ERROR);
        exit(1);
    }

    file.close();

}