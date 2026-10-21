#include <cstdlib>
#include <ctime>

#include "cpu.h"
#include "logger.h"
#include <string>

TCpu::TCpu (TChip8* machine)
{
    m_machine=machine;
    m_logger = TLogger::getInstance();

}

TCpu::~TCpu()
{


}

void TCpu::init()
{

    m_pcreg=PC_START;
    m_current_op=0;
    m_sp_reg=0;
    m_ireg=0;
    m_instruction =0;

    for(auto i=0; i<NUM_V_REGISTERS;i++)
        m_reg[i]=0;

}

void TCpu::fetch()
{
    m_current_op=((uint16_t)m_machine->m_ram[m_pcreg]<< 8) | m_machine-> m_ram[m_pcreg+1];
    m_pcreg += 2;
}


void TCpu::decode()
{

    m_instruction=m_current_op >> 12;

}

void TCpu::execute()
{
    m_logger->log("INST " + std::to_string(m_instruction), ELogLevel::DEBUG);
    
    switch(m_instruction)
    {
        case 0x0:
        case 0x1: 
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF: m_logger->log("INSTRUCTION OPCODE NO IMPLEMENTADA CHACHO!!!", ELogLevel::ERROR); exit(1); break;
	}
}

void TCpu::deinit()
{

}

//funciones privadas


  //DECODIFICADORES DE GRUPO
    // Para estos grupos el primer dígito no basta: miran el resto del opcode
    // y llaman a la instrucción concreta (un segundo switch).
    // 0???  → mira el último byte   (00E0, 00EE)
    void TCpu::decodifica_instruccion_0()
    {
         switch(m_current_op & 0xFF)
    {
        case 0xE0: limpiar_pantalla(); break;
        case 0xEE: devuelve_subrutina(); break;
        default: m_logger->log("Instruction 0 with code " + std::to_string(m_current_op & 0xFF), ELogLevel::ERROR);
    }
    };
    // 8XY?  → mira el último dígito (8XY0 ... 8XYE)
    void TCpu::decodifica_instruccion_8(){
             switch(m_current_op & 0xF)
    {
         switch (m_current_op & 0x000F)   // último dígito: el "?" de 8XY?
    {
        case 0x0: ajusta_VX_a_VY();          break;   // 8XY0
        case 0x1: ajusta_VX_a_VX_o_VY();     break;   // 8XY1
        case 0x2: ajusta_VX_a_VX_y_VY();     break;   // 8XY2
        case 0x3: ajusta_VX_a_VX_xor_VY();   break;   // 8XY3
        case 0x4: anade_VY_a_VX();           break;   // 8XY4
        case 0x5: resta_VY_a_VX();           break;   // 8XY5
        case 0x6: desplaza_VX_derecha();     break;   // 8XY6
        case 0x7: ajusta_VX_a_VY_menos_VX(); break;   // 8XY7
        case 0xE: desplaza_VX_izquierda();   break;   // 8XYE
        default:
            m_logger->log("Instruccion 8 con codigo desconocido: " + std::to_string(m_current_op & 0xF),
                          ELogLevel::ERROR);
            break;
    }
    }

    };
    // EX??  → mira el último byte   (EX9E, EXA1)
    void TCpu::decodifica_instruccion_E(){
        switch(m_current_op & 0xFF)
    {
        case 0x009E: saltar_instruccion_si_tecla_pulsada_almacenada_VX(); break;
        case 0x00A1: saltar_instruccion_si_tecla_no_pulsada_almacenada_VX(); break;
        default: m_logger->log("Instruction E with code " + std::to_string(m_current_op & 0xFF), ELogLevel::ERROR);
    }
    };
    // FX??  → mira el último byte   (FX07 ... FX65)
    void TCpu::decodifica_instruccion_F(){
          switch(m_current_op & 0xFF)
    {
        case 0x0007: carga_VX_con_retardo(); break;
        case 0x000A: espera_tecla(); break;
        case 0x0015: ajusta_temporizador_retardo(); break;
        case 0x0018: ajusta_temporizador_sonido(); break;
        case 0x001E: anade_VX_a_I(); break;
        case 0x0029: ajusta_I_a_posicion_VX(); break;
        case 0x0033: guardar_BCD_VX(); break;
        case 0x0055: guardar_desde_V0_a_VX(); break;
        case 0x0065: rellena_valores_desde_V0_a_VX(); break;
        default: m_logger->log("Instruction F with code " + std::to_string(m_current_op & 0xFF), ELogLevel::ERROR);
    }
    };

    //INSTRUCCIONES AQUI
    // Notación: NNN = 12 bits (dirección), NN = 8 bits (valor), N = 4 bits
    // 00E0
    void TCpu::limpiar_pantalla(){

        for(int i=0;i<SCREEN_HEIGHT;i++)
            for(int j=0;j<SCREEN_WIDTH;j++)
              m_machine->m_screen[i][j]=0;
    };
    // 00EE
    void TCpu::devuelve_subrutina()
    {
        m_sp_reg--;
        m_pcreg=m_machine->m_stack[m_sp_reg];

    };
    // 1NNN
    void TCpu::salta_a_direccion(){
        m_pcreg=m_current_op & 0x0FFF;

    };
    // 2NNN
    void TCpu::llama_subrutina_NNN(){

        uint16_t nnn= m_current_op & 0x0FFF;
        m_machine->m_stack[m_sp_reg]= m_pcreg;
        m_sp_reg++;
        m_pcreg=nnn;

    };
    // 3XNN
    void TCpu::saltar_instruccion_VX_igual_NN()
    {
          uint8_t value = m_current_op & 0xFF;
            uint8_t reg = (m_current_op >> 8) & 0x0F;

        if(m_reg[reg] == value)
         m_pcreg += 2;

    };
    // 4XNN
    void TCpu::saltar_instruccion_VX_no_igual_NN(){
        
    uint8_t value = m_current_op & 0xFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    if(m_reg[reg] != value)
        m_pcreg += 2;

    };
    // 5XY0
    void TCpu::saltar_instruccion_VX_igual_VY(){

         uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    if(m_reg[reg_x] == m_reg[reg_y])
        m_pcreg += 2;



    };
    // 6XNN
    void TCpu::ajusta_VX_a_NN()
    {
    uint8_t value = m_current_op & 0xFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    m_reg[reg] = value;

    };
    // 7XNN
    void TCpu::anade_NN_a_VX(){
    uint8_t value = m_current_op & 0xFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    m_reg[reg] += value;

    };
    // 8XY0
    void TCpu::ajusta_VX_a_VY(){
        uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;
        m_reg[reg_x]=m_reg[reg_y];
    };
    // 8XY1
    void TCpu::ajusta_VX_a_VX_o_VY(){
          uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;

        m_reg[reg_x]|=m_reg[reg_y];

    };
    // 8XY2
    void TCpu::ajusta_VX_a_VX_y_VY(){
         uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;
        
        m_reg[reg_x]&=m_reg[reg_y];

    };
    // 8XY3
    void TCpu::ajusta_VX_a_VX_xor_VY(){
         uint8_t reg_x = (m_current_op >> 8) & 0x0F;
            uint8_t reg_y = (m_current_op >> 4) & 0x0F;

            m_reg[reg_x] ^= m_reg[reg_y];

    };
    // 8XY4
    void TCpu::anade_VY_a_VX(){

         uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;
        uint16_t add = m_reg[reg_x] + m_reg[reg_y];
                if(add > 0xFF)
            m_reg[0xF] = 1;
        else
            m_reg[0xF] = 0;

        m_reg[reg_x] = add & 0xFF;
    };
    // 8XY5
    void TCpu::resta_VY_a_VX(){
         uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;
        
        if(m_reg[reg_x] > m_reg[reg_y])
            m_reg[0xF] = 1;
        else
            m_reg[0xF] = 0;

        m_reg[reg_x] -= m_reg[reg_y];


    };
    // 8XY6
    void TCpu::desplaza_VX_derecha(){

         uint8_t reg = (m_current_op >> 8) & 0x0F;

            if(m_reg[reg] % 2 == 1 )
                m_reg[0xF] = 1;
            else
                m_reg[0xF] = 0;

            m_reg[reg] >>= 1;
    };
    // 8XY7
    void TCpu::ajusta_VX_a_VY_menos_VX(){
        uint8_t reg_x = (m_current_op >> 8) & 0x0F;
        uint8_t reg_y = (m_current_op >> 4) & 0x0F;

        if(m_reg[reg_y] > m_reg[reg_x])
            m_reg[0xF] = 1;
        else
            m_reg[0xF] = 0;

        m_reg[reg_x] = m_reg[reg_y] - m_reg[reg_x];

    };
    // 8XYE
    void TCpu::desplaza_VX_izquierda(){

         uint8_t reg = (m_current_op >> 8) & 0x0F;

            if(m_reg[reg] % 0x80 == 1 )
                m_reg[0xF] = 1;
            else
                m_reg[0xF] = 0;

            m_reg[reg] <<= 1;

    };
    // 9XY0
    void TCpu::saltar_instruccion_si_VX_no_igual_VY(){

         uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    if(m_reg[reg_x] != m_reg[reg_y])
        m_pcreg += 2;
    };
    // ANNN
    void TCpu::ajusta_I_a_NNN(){

        m_ireg = m_current_op & 0x0FFF;
    };
    // BNNN
    void TCpu::salta_a_direccion_NNN_mas_V0(){

        uint16_t nnn = m_current_op & 0x0FFF;

    m_pcreg = nnn + m_reg[0];
    };
    // CXNN
    void TCpu::ajusta_VX_num_aleatorio(){
        uint8_t reg = (m_current_op >> 8) & 0x0F;
        uint8_t kk = m_current_op & 0xFF;
        uint8_t randNum = rand() % 256;

        m_reg[reg] = randNum & kk;
    };
    // DXYN
    void TCpu::dibuja_sprite(){

    uint8_t v_reg_x = (m_current_op & 0x0F00) >> 8;
    uint8_t v_reg_y = (m_current_op & 0x00F0) >> 4;   
    uint8_t sprite_height = m_current_op & 0x000F;
    uint8_t x_location = m_reg[v_reg_x];
    uint8_t y_location = m_reg[v_reg_y];

    // Reset colision register
    m_reg[0xF] = 0;
    for(int y_coordinate = 0; y_coordinate < sprite_height; y_coordinate++)
    {
        uint8_t pixel = m_machine->m_ram[m_ireg + y_coordinate];
        for(int x_coordinate = 0; x_coordinate < 8; x_coordinate++)
        {
            if((pixel & (0x80 >> x_coordinate)) != 0)
            { 
                if(m_machine->m_screen[y_location + y_coordinate][x_location + x_coordinate] == 1)
                    m_reg[0xF] = 1;
                
                m_machine->m_screen[y_location + y_coordinate][x_location + x_coordinate] ^= 0x1;
            }
        }
    }

    };
    // EX9E
    void TCpu::saltar_instruccion_si_tecla_pulsada_almacenada_VX(){
       uint8_t reg = (m_current_op >> 8) & 0x0F;
    uint8_t val = m_reg[reg];

    if(m_machine->m_keys[val] != 0)
        m_pcreg += 2;

    };
    // EXA1
    void TCpu::saltar_instruccion_si_tecla_no_pulsada_almacenada_VX(){

        uint8_t reg = (m_current_op >> 8) & 0x0F;
            uint8_t val = m_reg[reg];

            if(m_machine->m_keys[val] == 0)
                m_pcreg += 2;
    };
    // FX07  VX = temporizador de retardo
    void TCpu::carga_VX_con_retardo(){
        uint8_t reg = (m_current_op >> 8) & 0x0F;
            m_reg[reg] = m_machine->m_delay_timer;

    };
    // FX0A  espera (bloquea) hasta que se pulse una tecla y la guarda en VX
    void TCpu::espera_tecla(){
         uint8_t reg = (m_current_op >> 8) & 0x0F;
    bool key_pressed = false;

    for(int i=0; i<NUM_KEYS; i++)
    {
        if(m_machine->m_keys[i] != 0)
        {
            m_reg[reg] = i;
            key_pressed = true;
        }
    }

    if(!key_pressed)
        m_pcreg -= 2;

    };
    // FX15  temporizador de retardo = VX
    void TCpu::ajusta_temporizador_retardo(){
        uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_machine->m_delay_timer = m_reg[reg];

    };
    // FX18  temporizador de sonido = VX
    void TCpu::ajusta_temporizador_sonido(){
   uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_machine->m_sound_timer = m_reg[reg];

    };
    // FX1E
    void TCpu::anade_VX_a_I(){
        uint8_t reg = (m_current_op >> 8) & 0x0F;
        m_ireg += m_reg[reg];
                
    };
    // FX29
    void TCpu::ajusta_I_a_posicion_VX(){

        uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_ireg = m_reg[reg] * 0x5;
    };
    // FX33  centenas, decenas y unidades de VX en I, I+1, I+2
    void TCpu::guardar_BCD_VX(){
        uint8_t reg = (m_current_op >> 8) & 0x0F;

        m_machine->m_ram[m_ireg] = m_reg[reg] / 100;
        m_machine->m_ram[m_ireg+1] = (m_reg[reg] / 10) % 10;
        m_machine->m_ram[m_ireg+1] = (m_reg[reg] % 100) % 10;

    };
    // FX55
    void TCpu::guardar_desde_V0_a_VX(){
        uint8_t reg = (m_current_op >> 8) & 0x0F;

            for(int i=0; i<=reg; i++)
                m_machine->m_ram[m_ireg + i] = m_reg[i];

            m_ireg += (reg + 1);
    };
    // FX65
    void TCpu::rellena_valores_desde_V0_a_VX(){

        uint8_t reg = (m_current_op >> 8) & 0x0F;

    for(int i=0; i<=reg; i++)
        m_reg[i] = m_machine->m_ram[m_ireg + i];

    m_ireg += (reg + 1);
    };
