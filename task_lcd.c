/*
 * task_lcd.c
 *
 *  Created on: Sep 11, 2020
 *      Author: Joe Krachey, Eliot Pickhardt, Bill Lee
 */

#include <task_lcd.h>

const char **word;
int letters_found[5] = { -1, -1, -1, -1, -1 }; // for displaying found letters

/**
 * Task in charge of drawing images on the LCD, as well as the gameplay logic
 */
void Task_LCD(void *pvParameters)
{
    lcd_draw_rectangle(66, 66, 132, 132, LCD_COLOR_BLACK);
    BaseType_t status;
    Crystalfontz128x128_Init(); //initialize the LCD
    float light = 0; //for light sensor data
    bool color_stage = true; //for the pre-game color determination
    bool game_over = false; //game variable
    uint16_t color = LCD_COLOR_BLACK; //determines the color of hangman
    ece353_MKII_S1_Init(); //Intializes S1
    ece353_MKII_S2_Init(); //Initializes S2
    uint16_t random_gen = 0; //for the random word selection
    bool letters_selected[26] = { false }; //if each letter has been selected
    uint8_t current_letter; //a = 0, b = 1, c = 2, ... , y = 24, z = 25
    bool won = false;
    int accelerometer_direction = 0; //right = 1, left = -1;
    uint8_t hangman_state = 1;

    while (1)
    {
        won = false;
        lcd_draw_rectangle(66, 66, 132, 132, LCD_COLOR_BLACK);

        //Initial color selection stage.
        while (color_stage)
        {
            status = xSemaphoreGive(Sem_Light_Sensor); // Let the light sensor read data
            status = xSemaphoreGive(Sem_S1); // For detecting button presses (debounce)

            //Wait for Light Sensor to add data to the queue
            status = xQueueReceive(Queue_Task_Light_Sensor, &light,
                                   portMAX_DELAY); //Light data
            status = xQueueReceive(Queue_Task_Light_Sensor, &random_gen,
                                   portMAX_DELAY); //Probe data for RBG

            //color changes according to the light sensor input.
            color = calculate_color(light);

            //Draw letters on screen
            lcd_draw_image(42, 15, Small_Width, Small_Height,
                           (Small_Letters + 54 * 15), color, LCD_COLOR_BLACK); //p
            lcd_draw_image(58, 15, Small_Width, Small_Height,
                           (Small_Letters + 54 * 8), color, LCD_COLOR_BLACK); //i
            lcd_draw_image(74, 15, Small_Width, Small_Height,
                           (Small_Letters + 54 * 2), color, LCD_COLOR_BLACK); //c
            lcd_draw_image(92, 15, Small_Width, Small_Height,
                           (Small_Letters + 54 * 10), color, LCD_COLOR_BLACK); //k
            lcd_draw_image(42, 35, Small_Width, Small_Height,
                           (Small_Letters + 54 * 24), color, LCD_COLOR_BLACK); //y
            lcd_draw_image(58, 35, Small_Width, Small_Height,
                           (Small_Letters + 54 * 14), color, LCD_COLOR_BLACK); //o
            lcd_draw_image(74, 35, Small_Width, Small_Height,
                           (Small_Letters + 54 * 20), color, LCD_COLOR_BLACK); //u
            lcd_draw_image(92, 35, Small_Width, Small_Height,
                           (Small_Letters + 54 * 17), color, LCD_COLOR_BLACK); //r
            lcd_draw_image(26, 55, Small_Width, Small_Height,
                           (Small_Letters + 54 * 2), color, LCD_COLOR_BLACK); //c
            lcd_draw_image(46, 55, Small_Width, Small_Height,
                           (Small_Letters + 54 * 14), color, LCD_COLOR_BLACK); //o
            lcd_draw_image(66, 55, Small_Width, Small_Height,
                           (Small_Letters + 54 * 11), color, LCD_COLOR_BLACK); //l
            lcd_draw_image(86, 55, Small_Width, Small_Height,
                           (Small_Letters + 54 * 14), color, LCD_COLOR_BLACK); //o
            lcd_draw_image(106, 55, Small_Width, Small_Height,
                           (Small_Letters + 54 * 17), color, LCD_COLOR_BLACK); //r

            lcd_draw_image(58, 99, hangman_initWidthPixels, hangman_initHeightPixels,
                           hangman_initBitmaps, color, LCD_COLOR_BLACK); //stand

            //S1 is pressed and exiting the color state.
            if (ece353_MKII_S1())
            {
                status = xSemaphoreTake(Sem_S1, portMAX_DELAY); //wait for button debounce
                status = xSemaphoreTake(Sem_Light_Sensor, portMAX_DELAY); //Wait for light sensor reading to complete

                while (ece353_MKII_S1())
                {
                    //wait until the button gets released.
                }

                color_stage = false; //exit color select stage
                game_over = false; //enter gameplay stage
            }
            vTaskDelay(pdMS_TO_TICKS(2));

        }

        vTaskDelay(pdMS_TO_TICKS(500)); //wait .5s before starting game

        //variable resetting before game start
        current_letter = 0;
        lcd_draw_rectangle(66, 66, 132, 132, LCD_COLOR_BLACK);
        pick_word(random_gen);
        hangman_state = 1;
        memset(letters_selected, 0, 26 * sizeof(bool));
        memset(letters_found, -1, 5 * sizeof(int));

        //Box around selection letter.
        lcd_draw_rectangle(100, 32, 54, 56, color);
        lcd_draw_rectangle(100, 32, 30, 56, LCD_COLOR_WHITE);
        lcd_draw_rectangle(100, 33, 54, 34, LCD_COLOR_WHITE);
        lcd_draw_rectangle(100, 11, 46, 6, LCD_COLOR_BLACK);

        //tilt!
        lcd_draw_image(30, 95, Small_Width, Small_Height,
                                     Small_Letters + (54 * 19), LCD_COLOR_WHITE,
                                     LCD_COLOR_BLACK);//t
        lcd_draw_image(50, 95, Small_Width, Small_Height,
                                             Small_Letters + (54 * 8), LCD_COLOR_WHITE,
                                             LCD_COLOR_BLACK);//i
        lcd_draw_image(70, 95, Small_Width, Small_Height,
                                             Small_Letters + (54 * 11), LCD_COLOR_WHITE,
                                             LCD_COLOR_BLACK);//l
        lcd_draw_image(90, 95, Small_Width, Small_Height,
                                             Small_Letters + (54 * 19), LCD_COLOR_WHITE,
                                             LCD_COLOR_BLACK);//t

        lcd_draw_rectangle(104, 92, 4, 14, LCD_COLOR_WHITE);
        lcd_draw_rectangle(104, 95, 4, 2, LCD_COLOR_BLACK);

        //gameplay loop
        while (!game_over)
        {
            status = xSemaphoreGive(Sem_S2); //For detecting button presses
            status = xSemaphoreGive(Sem_S1);

            //depending on how many incorrect guesses the player has made, display correct image
            if (hangman_state == 1)
            {
                lcd_draw_image(35, 43, hangman_1WidthPixels,
                               hangman_1HeightPixels, hangman_1Bitmaps, color,
                               LCD_COLOR_BLACK);
            }
            else if (hangman_state == 2)
            {
                lcd_draw_image(35, 43, hangman_2WidthPixels,
                               hangman_2HeightPixels, hangman_2Bitmaps, color,
                               LCD_COLOR_BLACK);
            }
            else if (hangman_state == 3)
            {
                lcd_draw_image(35, 43, hangman_3WidthPixels,
                               hangman_3HeightPixels, hangman_3Bitmaps, color,
                               LCD_COLOR_BLACK);
            }
            else if (hangman_state == 4)
            {
                lcd_draw_image(35, 43, hangman_4WidthPixels,
                               hangman_4HeightPixels, hangman_4Bitmaps, color,
                               LCD_COLOR_BLACK);
            }
            else if (hangman_state == 5)
            {
                lcd_draw_image(35, 43, hangman_5WidthPixels,
                               hangman_5HeightPixels, hangman_5Bitmaps, color,
                               LCD_COLOR_BLACK);
            }
            else if (hangman_state == 6)
            {
                lcd_draw_image(35, 43, hangman_6WidthPixels,
                               hangman_6HeightPixels, hangman_6Bitmaps, color,
                               LCD_COLOR_BLACK);
            }

            //representation of the selection letter.
            lcd_draw_image(100, 35, Big_Width, Big_Height,
                           (Big_Letters + 246 * current_letter), color,
                           LCD_COLOR_BLACK);


            //draw underscores for letters not found
            lcd_draw_image(15, 128, Underscore_Width, Underscore_Height,
                           Underscore, color, LCD_COLOR_BLACK);
            lcd_draw_image(40, 128, Underscore_Width, Underscore_Height,
                           Underscore, color, LCD_COLOR_BLACK);
            lcd_draw_image(65, 128, Underscore_Width, Underscore_Height,
                           Underscore, color, LCD_COLOR_BLACK);
            lcd_draw_image(90, 128, Underscore_Width, Underscore_Height,
                           Underscore, color, LCD_COLOR_BLACK);
            lcd_draw_image(115, 128, Underscore_Width, Underscore_Height,
                           Underscore, color, LCD_COLOR_BLACK);

            //if S2 pressed, reset to color select
            if (ece353_MKII_S2())
            {
                status = xSemaphoreTake(Sem_S2, portMAX_DELAY);
                while (ece353_MKII_S2())
                {
                    //wait for release
                }
                color_stage = true;
                game_over = true;
            }

            //if S1 pressed and current letter not selected, select letter
            else if (ece353_MKII_S1() && !letters_selected[current_letter])
            {
                status = xSemaphoreTake(Sem_S1, portMAX_DELAY);
                letters_selected[current_letter] = true;

                if (letter_correct(current_letter)) //if letter is in word
                {
                    //find which spot(s) letter is in and display
                    if (letters_list[current_letter] == word[0][0])
                    {
                        letters_found[0] = current_letter;
                        lcd_draw_image(15, 118, Small_Width, Small_Height,
                                       (Small_Letters + 54 * current_letter),
                                       color, LCD_COLOR_BLACK);
                    }
                    if (letters_list[current_letter] == word[0][1])
                    {
                        letters_found[1] = current_letter;
                        lcd_draw_image(40, 118, Small_Width, Small_Height,
                                       (Small_Letters + 54 * current_letter),
                                       color, LCD_COLOR_BLACK);
                    }
                    if (letters_list[current_letter] == word[0][2])
                    {
                        letters_found[2] = current_letter;
                        lcd_draw_image(65, 118, Small_Width, Small_Height,
                                       (Small_Letters + 54 * current_letter),
                                       color, LCD_COLOR_BLACK);
                    }
                    if (letters_list[current_letter] == word[0][3])
                    {
                        letters_found[3] = current_letter;
                        lcd_draw_image(90, 118, Small_Width, Small_Height,
                                       (Small_Letters + 54 * current_letter),
                                       color, LCD_COLOR_BLACK);
                    }
                    if (letters_list[current_letter] == word[0][4])
                    {
                        letters_found[4] = current_letter;
                        lcd_draw_image(115, 118, Small_Width, Small_Height,
                                       (Small_Letters + 54 * current_letter),
                                       color, LCD_COLOR_BLACK);
                    }
                }

                else //if letter is not in word
                {
                    hangman_state++; //increment incorrect guesses
                    status = xSemaphoreGive(Sem_Buzzer); //sound buzzer for .5s
                }
            }

            //read accelerometer data
            status = xQueueReceive(Queue_Task_Accelerometer,
                                   &accelerometer_direction, 0); //don't wait in case no movement was detected

            //if accelerometer movement was detected
            if (status == pdTRUE && accelerometer_direction != 0)
            {
                //depending on direction of movement, change current letter
                if (accelerometer_direction < 0)
                {
                    if (current_letter == 0)
                        current_letter = 26;
                    current_letter = current_letter - 1;
                }
                else if (accelerometer_direction > 0)
                {
                    current_letter = (current_letter + 1) % 26;
                }
                accelerometer_direction = 0;
            }

            //check win condition
            if (check_win())
            {
                game_over = true;
                won = true;
            }
            //check loss condition
            if (hangman_state == 7)
            {
                game_over = true;
                won = false;
            }
            vTaskDelay(pdMS_TO_TICKS(2));
        }

        //Variable reset for endgame screen
        status = xSemaphoreGive(Sem_S2);
        lcd_draw_rectangle(66, 66, 132, 132, LCD_COLOR_BLACK);

        //endgame loop
        while (game_over)
        {
            lcd_draw_image(42, 15, Small_Width, Small_Height,
                           Small_Letters + (54 * 24), LCD_COLOR_RED,
                           LCD_COLOR_BLACK); //Y
            lcd_draw_image(64, 15, Small_Width, Small_Height,
                           Small_Letters + (54 * 14), LCD_COLOR_RED,
                           LCD_COLOR_BLACK); //O
            lcd_draw_image(86, 15, Small_Width, Small_Height,
                           Small_Letters + (54 * 20), LCD_COLOR_RED,
                           LCD_COLOR_BLACK); //U
            if (won)
            {
                lcd_draw_image(42, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 22), LCD_COLOR_BLUE,
                               LCD_COLOR_BLACK); //W
                lcd_draw_image(64, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 8), LCD_COLOR_BLUE,
                               LCD_COLOR_BLACK); //I
                lcd_draw_image(86, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 13), LCD_COLOR_BLUE,
                               LCD_COLOR_BLACK); //N
                lcd_draw_image(66, 85, hangman_8WidthPixels,
                               hangman_8HeightPixels, hangman_8Bitmaps, color,
                               LCD_COLOR_BLACK);

            }
            else
            {
                lcd_draw_image(31, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 11), LCD_COLOR_RED,
                               LCD_COLOR_BLACK); //L
                lcd_draw_image(53, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 14), LCD_COLOR_RED,
                               LCD_COLOR_BLACK); //O
                lcd_draw_image(75, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 18), LCD_COLOR_RED,
                               LCD_COLOR_BLACK); //S
                lcd_draw_image(97, 35, Small_Width, Small_Height,
                               Small_Letters + (54 * 4), LCD_COLOR_RED,
                               LCD_COLOR_BLACK); //E

                lcd_draw_image(66, 85, hangman_7WidthPixels,
                               hangman_7HeightPixels, hangman_7Bitmaps, color,
                               LCD_COLOR_BLACK);
            }

            //if S2 pressed, reset to color select stage
            if (ece353_MKII_S2())
            {
                status = xSemaphoreTake(Sem_S2, portMAX_DELAY);
                game_over = false;
                color_stage = true;
                won = false;
            }
            vTaskDelay(pdMS_TO_TICKS(2));
        }
    }
}

/**
 * Helper function to check if the game has been won
 */
bool check_win(void)
{
    //iterate through letters found to see if there are any not found
    int l;
    for (l = 0; l < 6; l++)
    {
        if (letters_found[l] == -1)
            return false;
    }
    return true;
}

/**
 * Helper function to determine whether a selected letter is found in the word
 */
bool letter_correct(uint8_t letter)
{
    //iterate through letters in the word to see if selected letter appears
    int c;
    for (c = 0; c < 6; c++)
    {
        if (letters_list[letter] == word[0][c])
            return true;
    }
    return false;
}

/**
 * Helper function to calculate the color depending on the received light sensor value
 */
uint16_t calculate_color(float light)
{
    //Take light value and assign a color
    if (light < 100)
    {
        return LCD_COLOR_MAGENTA;
    }
    else if (light < 200)
    {
        return LCD_COLOR_RED;
    }
    else if (light < 300)
    {
        return LCD_COLOR_GREEN;
    }
    else if (light < 400)
    {
        return LCD_COLOR_BLUE;
    }
    else if (light < 500)
    {
        return LCD_COLOR_YELLOW;
    }
    else if (light < 600)
    {
        return LCD_COLOR_WHITE;
    }
    else
        return LCD_COLOR_BROWN;
}

/**
 * Pick a word based on the "random" LSBs of the light sensor data
 */
void pick_word(uint16_t index)
{
    index = (index & 0x7F); //only use last 7 bits since there are 128 words
    word = (word_list + index);
}

/* ****************************************************************************
 * Used to configure the 5 pins that control the LCD interface on the MKII.
 *
 * CLK and MOSI will be configured as their Primary Module Function
 *
 * CS, C/D, and RST will be configured at output pins
 ******************************************************************************/
void HAL_LCD_PortInit(void)
{
    // ADD CODE 

    // LCD_SCK
    LCD_SCK_PORT->SEL0 |= LCD_SCK_PIN;
    LCD_SCK_PORT->SEL1 &= ~LCD_SCK_PIN;

    // LCD_MOSI
    LCD_MOSI_PORT->SEL0 |= LCD_MOSI_PIN;
    LCD_MOSI_PORT->SEL1 &= ~LCD_MOSI_PIN;

    // LCD_CS
    LCD_CS_PORT->DIR |= LCD_CS_PIN;
    // LCD_RST
    LCD_RST_PORT->DIR |= LCD_RST_PIN;
    // LCD_DC
    LCD_DC_PORT->DIR |= LCD_DC_PIN;
}
;
/* ****************************************************************************
 * Used to configure the eUSCIB Interface as a 3-Wire SPI Interface
 *
 ******************************************************************************/
void HAL_LCD_SpiInit(void)
{
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;   // Put eUSCI state machine in reset

    // ADD CODE to define the behavior of the eUSCI_B0 as a SPI interface
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_CKPH |
    EUSCI_B_CTLW0_MSB |
    EUSCI_B_CTLW0_MST |
    EUSCI_B_CTLW0_MODE_0 |
    EUSCI_B_CTLW0_SYNC |
    EUSCI_B_CTLW0_SSEL__SMCLK |
    //EUSCI_B_CTLW0_STEM|
            EUSCI_B_CTLW0_SWRST;

    // ADD CODE to set the SPI Clock to 12MHz.
    //
    // Divide clock speed by 2 (24MHz/2) = 12 MHz
    //fBitClock = fBRCLK/(UCBRx+1).
    EUSCI_B0->BRW = 1;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Initialize USCI state machine

    // set the chip select low
    // The chip select will be held low forever!  There is only 1 device (LCD) 
    // connected to the SPI device, so we will leave it activated all the time.
    LCD_CS_PORT->OUT &= ~LCD_CS_PIN;

    // Set the DC pin high (put it in data mode)
    LCD_DC_PORT->OUT |= LCD_DC_PIN;

}

//*****************************************************************************
//
// Writes a command to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void HAL_LCD_writeCommand(uint8_t command)
{
    // ADD CODE

    // Set to command mode -- DC PIN Set to 0
    LCD_DC_PORT->OUT &= ~LCD_DC_PIN;

    // Busy wait while the data is being transmitted. Check the STATW register and see if it is busy 
    while (EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY)
    {

    };

    // Transmit data
    EUSCI_B0->TXBUF = command;

    // Busy wait while the data is being transmitted. Check the STATW register and see if it is busy 
    while (EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY)
    {

    };

    // Set back to data mode, set the DC pin high
    LCD_DC_PORT->OUT |= LCD_DC_PIN;
}

//*****************************************************************************
//
// Writes a data to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void HAL_LCD_writeData(uint8_t data)
{
    // ADD CODE

    // Busy wait while the data is being transmitted. Check the STATW register and see if it is busy 
    while (EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY)
    {

    };

    // Transmit data
    EUSCI_B0->TXBUF = data;

    // Busy wait while the data is being transmitted. Check the STATW register and see if it is busy 
    while (EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY)
    {

    };
}

//*****************************************************************************
// Code adapted from TI LCD driver library
//*****************************************************************************
void Crystalfontz128x128_SetDrawFrame(uint16_t x0, uint16_t y0, uint16_t x1,
                                      uint16_t y1)
{

    // ADD CODE
    // Write the CM_CASET command followed by the 4 bytes of data
    // used to set the Column start and end rows.
    HAL_LCD_writeCommand(CM_CASET);
    HAL_LCD_writeData(x0 >> 8);
    HAL_LCD_writeData(x0);
    HAL_LCD_writeData(x1 >> 8);
    HAL_LCD_writeData(x1);

    // ADD CODE
    // Write the CM_RASET command followed by the 4 bytes of data
    // used to set the Row start and end rows.
    HAL_LCD_writeCommand(CM_RASET);
    HAL_LCD_writeData(y0 >> 8);
    HAL_LCD_writeData(y0);
    HAL_LCD_writeData(y1 >> 8);
    HAL_LCD_writeData(y1);
}

//*****************************************************************************
// Code adapted from TI LCD driver library
//
//! Initializes the display driver.
//!
//! This function initializes the ST7735 display controller on the panel,
//! preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
void Crystalfontz128x128_Init(void)
{
    int i;
    int j;
    uint8_t upper8 = 0x00;
    uint8_t lower8 = 0x00;

    HAL_LCD_PortInit();
    HAL_LCD_SpiInit();

    LCD_RST_PORT->OUT &= ~LCD_RST_PIN;
    HAL_LCD_delay(50);

    LCD_RST_PORT->OUT |= LCD_RST_PIN;
    HAL_LCD_delay(120);

    HAL_LCD_writeCommand(CM_SLPOUT);
    HAL_LCD_delay(200);

    HAL_LCD_writeCommand(CM_GAMSET);
    HAL_LCD_writeData(0x04);

    HAL_LCD_writeCommand(CM_SETPWCTR);
    HAL_LCD_writeData(0x0A);
    HAL_LCD_writeData(0x14);

    HAL_LCD_writeCommand(CM_SETSTBA);
    HAL_LCD_writeData(0x0A);
    HAL_LCD_writeData(0x00);

    HAL_LCD_writeCommand(CM_COLMOD);
    HAL_LCD_writeData(0x05);
    HAL_LCD_delay(10);

    HAL_LCD_writeCommand(CM_MADCTL);
    HAL_LCD_writeData(CM_MADCTL_MX | CM_MADCTL_MY | CM_MADCTL_BGR);

    HAL_LCD_writeCommand(CM_NORON);

    Crystalfontz128x128_SetDrawFrame(0, 0, LCD_HORIZONTAL_MAX,
    LCD_VERTICAL_MAX);
    HAL_LCD_writeCommand(CM_RAMWR);

    for (i = 0; i < LCD_VERTICAL_MAX; i++)
    {

        for (j = 0; j < LCD_HORIZONTAL_MAX; j++)
        {
            HAL_LCD_writeData(upper8);
            HAL_LCD_writeData(lower8);
        }
    }

    HAL_LCD_delay(10);

    HAL_LCD_writeCommand(CM_DISPON);

    HAL_LCD_writeCommand(CM_MADCTL);
    HAL_LCD_writeData(CM_MADCTL_MX | CM_MADCTL_MY | CM_MADCTL_BGR);
}

/*******************************************************************************
 * Function Name: lcd_draw_rectangle
 ********************************************************************************
 * Summary: draws a filled rectangle centered at the coordinates set by x, y
 * Returns:
 *  Nothing
 *******************************************************************************/
void lcd_draw_rectangle(uint16_t x, uint16_t y, uint16_t width_pixels,
                        uint16_t height_pixels, uint16_t fColor)
{
    uint16_t i, j;
    uint16_t x0;
    uint16_t x1;
    uint16_t y0;
    uint16_t y1;

    // Set the left Col to be the center point minus half the width
    x0 = x - (width_pixels / 2);

    // Set the Right Col to be the center point plus half the width
    x1 = x + (width_pixels / 2);

    // Account for a width that is an even number
    if ((width_pixels & 0x01) == 0x00)
    {
        x1--;
    }

    // Set the upper Row to be the center point minus half the height
    y0 = y - (height_pixels / 2);

    // Set the upper Row to be the center point minus half the height
    y1 = y + (height_pixels / 2);

    // Account for a height that is an  number
    if ((height_pixels & 0x01) == 0x00)
    {
        y1--;
    }

    // Set the boundry of the image to draw
    Crystalfontz128x128_SetDrawFrame(x0, y0, x1, y1);

    // Inform the LCD we are going to send image data
    HAL_LCD_writeCommand(CM_RAMWR);

    // Write out the image data
    for (i = 0; i < height_pixels; i++)
    {
        for (j = 0; j < width_pixels; j++)
        {
            HAL_LCD_writeData(fColor >> 8);
            HAL_LCD_writeData(fColor);
        }
    }

}

/*******************************************************************************
 * Function Name: lcd_draw_image
 ********************************************************************************
 * Summary: Prints an image centered at the coordinates set by x, y
 * Returns:
 *  Nothing
 *******************************************************************************/
void lcd_draw_image(uint16_t x_start, uint16_t y_start,
                    uint16_t image_width_pixels, uint16_t image_height_pixels,
                    const uint8_t *image, uint16_t fColor, uint16_t bColor)
{
    uint16_t i, j;
    uint8_t data;
    uint16_t byte_index;
    uint16_t bytes_per_row;
    uint16_t x0;
    uint16_t x1;
    uint16_t y0;
    uint16_t y1;

    x0 = x_start - (image_width_pixels / 2);
    x1 = x_start + (image_width_pixels / 2);
    if ((image_width_pixels & 0x01) == 0x00)
    {
        x1--;
    }

    y0 = y_start - (image_height_pixels / 2);
    y1 = y_start + (image_height_pixels / 2);
    if ((image_height_pixels & 0x01) == 0x00)
    {
        y1--;
    }

    Crystalfontz128x128_SetDrawFrame(x0, y0, x1, y1);
    HAL_LCD_writeCommand(CM_RAMWR);
    //lcd_set_pos(x0, x1, y0, y1);

    bytes_per_row = image_width_pixels / 8;
    if ((image_width_pixels % 8) != 0)
    {
        bytes_per_row++;
    }

    for (i = 0; i < image_height_pixels; i++)
    {
        for (j = 0; j < image_width_pixels; j++)
        {
            if ((j % 8) == 0)
            {
                byte_index = (i * bytes_per_row) + j / 8;
                data = image[byte_index];
            }
            if (data & 0x80)
            {
                HAL_LCD_writeData(fColor >> 8);
                HAL_LCD_writeData(fColor);
            }
            else
            {
                HAL_LCD_writeData(bColor >> 8);
                HAL_LCD_writeData(bColor);
            }
            data = data << 1;
        }
    }
}

