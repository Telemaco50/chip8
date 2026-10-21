#ifndef __CPU_H__
#define __CPU_H__

#include <cstdint>

#include "logger.h"
#include "Chip8.h"
#include <memory>
// #include 
#define NUM_V_REGISTERS 16
#define PC_START 0x200


class TCpu
{
    //Registers
    uint8_t m_reg[NUM_V_REGISTERS];
    uint16_t m_ireg;
    uint16_t m_pcreg;
    uint16_t m_sp_reg;


    //codigo de ayuda
    uint16_t m_current_op; //op en curso
    uint16_t m_instruction ;

    TChip8* m_machine;


    //DECODIFICADORES DE GRUPO
    // Para estos grupos el primer dígito no basta: miran el resto del opcode
    // y llaman a la instrucción concreta (un segundo switch).
    // 0???  → mira el último byte   (00E0, 00EE)
    void decodifica_instruccion_0();
    // 8XY?  → mira el último dígito (8XY0 ... 8XYE)
    void decodifica_instruccion_8();
    // EX??  → mira el último byte   (EX9E, EXA1)
    void decodifica_instruccion_E();
    // FX??  → mira el último byte   (FX07 ... FX65)
    void decodifica_instruccion_F();

    //INSTRUCCIONES AQUI
    // Notación: NNN = 12 bits (dirección), NN = 8 bits (valor), N = 4 bits
    // 00E0
    void limpiar_pantalla();
    // 00EE
    void devuelve_subrutina();
    // 1NNN
    void salta_a_direccion();
    // 2NNN
    void llama_subrutina_NNN();
    // 3XNN
    void saltar_instruccion_VX_igual_NN();
    // 4XNN
    void saltar_instruccion_VX_no_igual_NN();
    // 5XY0
    void saltar_instruccion_VX_igual_VY();
    // 6XNN
    void ajusta_VX_a_NN();
    // 7XNN
    void anade_NN_a_VX();
    // 8XY0
    void ajusta_VX_a_VY();
    // 8XY1
    void ajusta_VX_a_VX_o_VY();
    // 8XY2
    void ajusta_VX_a_VX_y_VY();
    // 8XY3
    void ajusta_VX_a_VX_xor_VY();
    // 8XY4
    void anade_VY_a_VX();
    // 8XY5
    void resta_VY_a_VX();
    // 8XY6
    void desplaza_VX_derecha();
    // 8XY7
    void ajusta_VX_a_VY_menos_VX();
    // 8XYE
    void desplaza_VX_izquierda();
    // 9XY0
    void saltar_instruccion_si_VX_no_igual_VY();
    // ANNN
    void ajusta_I_a_NNN();
    // BNNN
    void salta_a_direccion_NNN_mas_V0();
    // CXNN
    void ajusta_VX_num_aleatorio();
    // DXYN
    void dibuja_sprite();
    // EX9E
    void saltar_instruccion_si_tecla_pulsada_almacenada_VX();
    // EXA1
    void saltar_instruccion_si_tecla_no_pulsada_almacenada_VX();
    // FX07  VX = temporizador de retardo
    void carga_VX_con_retardo();
    // FX0A  espera (bloquea) hasta que se pulse una tecla y la guarda en VX
    void espera_tecla();
    // FX15  temporizador de retardo = VX
    void ajusta_temporizador_retardo();
    // FX18  temporizador de sonido = VX
    void ajusta_temporizador_sonido();
    // FX1E
    void anade_VX_a_I();
    // FX29
    void ajusta_I_a_posicion_VX();
    // FX33  centenas, decenas y unidades de VX en I, I+1, I+2
    void guardar_BCD_VX();
    // FX55
    void guardar_desde_V0_a_VX();
    // FX65
    void rellena_valores_desde_V0_a_VX();


    //Logging
    std::shared_ptr<TLogger> m_logger;

    public:
        TCpu(TChip8* machine);
        ~TCpu();
        void init();
        void fetch();
        void decode();
        void execute();
        void deinit();

};


#endif