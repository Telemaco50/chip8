#include "Chip8.h"

TChip8::TChip8()
{
    // m_logger= TLogger::getInstance();
    m_emulatorRunning=true;

}

TChip8::~TChip8()
{}

void TChip8::init(std::string rom_path)
{

    //Clear frame buffer
    for(auto i=0;i<SCREEN_HEIGHT;i++)
        for(auto j=0; j<SCREEN_WIDTH;j++)
            m_screen[i][j]=0;
    //Limpia stack
    for(auto i=0; i< STACK_SIZE;i++)
        m_stack[i]=0;

    //Limpia RAM
    for(auto i=0; i< TOTAL_RAM;i++)
        m_ram[i]=0;
    
    //Cargar fuente en memoria
    for(auto i=0; i< FONTSET_SIZE;i++)
        m_ram[i]=FONTSET[i];

    //Reset de temporizadores
    m_delay_timer =0;
    m_sound_timer=0;

    //Inicializa el teclado con todo sin pulsar

    for(auto i=0; i< NUM_KEYS;i++)
        m_keys[i]=0;
        m_key_pressed=false;
}

void TChip8::run()
{}

void TChip8::deinit()
{}