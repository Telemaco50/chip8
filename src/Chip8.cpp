#include <iostream>
#include <chrono>
#include <thread>

#include "chip8.h"
#include "cpu.h"
#include "logger.h"
#include <chrono>
TChip8::TChip8()
{
    m_logger= TLogger::getInstance();
    m_cpu= new TCpu(this);
    m_emulatorRunning=true;
    m_display=nullptr;
    m_keyboard=nullptr;

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


    m_cpu->init();
    m_loader=new TRomLoader();
    m_loader->loadRom(rom_path,m_ram+PROGRAM_START_ADDR);
    delete m_loader;

    //Inicializar el display
    m_display -> init();


    //Inicializar el teclado
    m_keyboard-> init();
}

void TChip8::run()
{

    using clock=std::chrono::high_resolution_clock;
    clock::time_point start,end;
    const std::chrono::milliseconds desired_cycle_time(1);

    int display_update_delay_time=0;

    while(m_emulatorRunning)
        {
            start= clock::now();
            m_cpu->fetch();
            m_cpu->decode();
            m_cpu->execute();
            m_display->draw(m_screen, SCREEN_WIDTH,SCREEN_HEIGHT);
            m_display->update();
            m_keyboard->update(m_keys, &m_emulatorRunning);

            if(m_delay_timer> 0)
                m_delay_timer--;

            if(m_sound_timer> 0)
                m_sound_timer--;

        }

        end= clock::now();

        std::chrono::duration<double, std::micro> loop_time = end - start;
        //Calcular el tiempo que ha pasado en millsecs
        auto elapsed_time=std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        //Calcular el tiempo que falta
        auto sleep_time = desired_cycle_time - elapsed_time;

        if(sleep_time.count()<0){
            std::this_thread::sleep_for(sleep_time);
        }
        display_update_delay_time++;


}

void TChip8::deinit()
{

    m_cpu->deinit();
    m_display ->deinit();
    m_keyboard->deinit();
}

void TChip8::setDisplay(TDisplay* display)
{
        m_display=display;
}

void TChip8::setKeyboard(TKeyboard* keyboard)
{
        m_keyboard=keyboard;
}