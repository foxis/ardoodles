
#define TLV_RST_LO  P1OUT &= ~BIT3
#define TLV_RST_HI  P1OUT |=  BIT3

#define CPU_RST_LO  P1OUT &= ~BIT1
#define CPU_RST_HI  P1OUT |=  BIT1

#define EE_CS_LO    P1OUT &= ~BIT4
#define EE_CS_HI    P1OUT |=  BIT4

#define FUJ_INT     (P1IN & BIT7)


#define FPGA_RST_LO P2OUT &= ~BIT0
#define FPGA_RST_HI P2OUT |=  BIT0

#define EE_SCK_LO   P2OUT &= ~BIT1
#define EE_SCK_HI   P2OUT |=  BIT1

#define MP2TS_EN_LO P2OUT &= ~BIT2
#define MP2TS_EN_HI P2OUT |=  BIT2

#define EE_SI_LO    P2OUT &= ~BIT7
#define EE_SI_HI    P2OUT |=  BIT7


#define FPGA_MISO   (P3IN & BIT2)



#define CMD_LO      P4OUT &= ~BIT6
#define CMD_HI      P4OUT |=  BIT6

#define MIPI_RST_LO P4OUT &= ~BIT1
#define MIPI_RST_HI P4OUT |=  BIT1

#define SCL         (P4IN & BIT4)
#define SCL_RLS     P4DIR &= ~BIT4
#define SCL_DRV     P4DIR |=  BIT4

#define SDA        (P4IN & BIT3)
#define SDA_RLS     P4DIR &= ~BIT3
#define SDA_DRV     P4DIR |=  BIT3

#define FPGA_DONE   (P4IN & BIT0)

#define I2C_DLY_TLV     5
#define I2C_DLY_MIPI    5
#define I2C_DLY_CAM     20


#define us20        40
#define us100       200
#define ms1         2000
#define ms10        20000
#define ms25        50000
