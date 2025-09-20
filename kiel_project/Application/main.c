//--------------------------------------------------------
// Application demonstrator: SNAKE game
//--------------------------------------------------------

#include "EDK_CM0.h"
#include "core_cm0.h"
#include "edk_driver.h"
#include "edk_api.h"

#include <stdio.h>
#include <stdlib.h>

// Game region
#define SCORE_X_START 55
#define SCORE_Y_START 4
#define left_boundary 0
#define right_boundary 99
#define top_boundary 0
#define bottom_boundary 119
#define boundary_thick 0

// Global variables
static char key;
static int score;
static int pause;
static int rocket_has_moved;
static int rocket_has_attacked;
static int rocket_attack_button_released;
static int alien_alive_status[3][5] = {{1, 1, 1, 1, 1},
                                       {1, 1, 1, 1, 1},
                                       {1, 1, 1, 1, 1}};

static int gamespeed;
static int speed_table[10] = {6, 9, 12, 15, 20, 25, 30, 35, 40, 100};

// Structure define
struct Alien {
    int alien_box_x;
    int alien_box_y;
    int display_start_index;
    int display_stop_index;
    int direction;
} alien;

struct Rocket {
    int x;
    int y;
    int lives;
    int direction;
    int shoot;
} rocket;

static int alien_attacks[3][20] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

static int rocket_attacks[3][20] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

static int alien_attack_x_arbiter;
static int alien_attack_y_arbiter;
static int alien_attack_delay;

int rocket_image[8][9] = {{BLACK, BLACK, BLACK, BLACK, BLUE, BLACK, BLACK, BLACK, BLACK},
                          {BLACK, BLACK, BLACK, BLACK, BLUE, BLACK, BLACK, BLACK, BLACK},
                          {BLACK, BLACK, BLACK, BLACK, BLUE, BLACK, BLACK, BLACK, BLACK},
                          {BLACK, BLACK, BLACK, BLUE, BLUE, BLUE, BLACK, BLACK, BLACK},
                          {BLUE, BLACK, BLACK, BLUE, BLUE, BLUE, BLACK, BLACK, BLUE},
                          {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE},
                          {BLUE, BLACK, RED, BLACK, BLUE, BLACK, RED, BLACK, BLUE},
                          {RED, BLACK, BLACK, BLACK, RED, BLACK, BLACK, BLACK, RED}};

int alien_image[9][9] = {{BLACK, BLACK, GREEN, BLACK, BLACK, BLACK, GREEN, BLACK, BLACK},
                         {BLACK, BLACK, BLACK, GREEN, BLACK, GREEN, BLACK, BLACK, BLACK},
                         {BLACK, BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK, BLACK},
                         {BLACK, GREEN, GREEN, BLACK, GREEN, BLACK, GREEN, GREEN, BLACK},
                         {BLACK, GREEN, BLACK, BLACK, GREEN, BLACK, BLACK, GREEN, BLACK},
                         {GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN},
                         {GREEN, BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK, GREEN},
                         {GREEN, BLACK, GREEN, BLACK, BLACK, BLACK, GREEN, BLACK, GREEN},
                         {BLACK, BLACK, BLACK, GREEN, BLACK, GREEN, BLACK, BLACK, BLACK}};

int small_life_icon[5][5] = {
    {RED, RED, BLACK, RED, RED},
    {RED, RED, RED, RED, RED},
    {RED, RED, RED, RED, RED},
    {BLACK, RED, RED, RED, BLACK},
    {BLACK, BLACK, RED, BLACK, BLACK}
};
int digit_bitmaps[10][5][5] = {
    {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}}, // 0
    {{0, 0, 1, 0, 0}, {0, 1, 1, 0, 0}, {1, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {1, 1, 1, 1, 1}}, // 1
    {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}}, // 2
    {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 1}}, // 3
    {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}}, // 4
    {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 1}}, // 5
    {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}}, // 6
    {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}}, // 7
    {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}}, // 8
    {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 1}}  // 9
};

int S_bitmap[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

int C_bitmap[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1}
};

int O_bitmap[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

int R_bitmap[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0},
    {1, 0, 1, 1, 1}
};

int E_bitmap[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1}
};

void DisplayCharacter(int char_bitmap[5][5], int start_x, int start_y) {
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (char_bitmap[i][j] == 1) {
                VGA_plot_pixel(start_x + j, start_y + i, WHITE);
            } else {
                VGA_plot_pixel(start_x + j, start_y + i, BLACK);
            }
        }
    }
}

void DisplayDigit(int digit, int start_x, int start_y) {
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (digit_bitmaps[digit][i][j] == 1) {
                VGA_plot_pixel(start_x + j, start_y + i, WHITE);
            } else {
                VGA_plot_pixel(start_x + j, start_y + i, BLACK);
            }
        }
    }
}

void DisplayScoreText(int start_x, int start_y) {
    DisplayCharacter(S_bitmap, start_x, start_y);
    DisplayCharacter(C_bitmap, start_x + 6, start_y);
    DisplayCharacter(O_bitmap, start_x + 12, start_y);
    DisplayCharacter(R_bitmap, start_x + 18, start_y);
    DisplayCharacter(E_bitmap, start_x + 24, start_y);
}

void DisplayScore(int score, int start_x, int start_y) {
    char score_str[10];
    int length, i;

    // Display the text "Score"
    DisplayScoreText(start_x, start_y);

    // Convert score to string
    sprintf(score_str, "%d", score);
    length = strlen(score_str);

    // Display each digit
    for (i = 0; i < length; i++) {
        DisplayDigit(score_str[i] - '0', start_x + 30 + (i * 6), start_y); // Start position adjusted to leave space for "Score"
    }
}

//---------------------------------------------
// Game
//---------------------------------------------

void Game_Init(void) {
    int i, j;

    // Draw a game region
    clear_screen();
    rectangle(left_boundary, top_boundary, right_boundary, top_boundary + boundary_thick, WHITE);
    rectangle(left_boundary, top_boundary, left_boundary + boundary_thick, bottom_boundary, WHITE);
    rectangle(left_boundary, bottom_boundary, right_boundary, bottom_boundary + boundary_thick, WHITE);
    rectangle(right_boundary, top_boundary, right_boundary + boundary_thick, bottom_boundary + boundary_thick, WHITE);

    // Initialise data
    score = 0;
    gamespeed = speed_table[score];

    // Initialize the position of Rocket
    rocket.x = 50;
    rocket.y = 110;
    rocket.lives = 3;
    rocket.direction = 0;

    rocket_has_attacked = 0;
    rocket_attack_button_released = 0;

    for (j = 0; j < 3; j++)
        for (i = 0; i < 20; i++)
            rocket_attacks[j][i] = 0;

    // Initialize the parameters of Aliens
    alien.alien_box_x = 6;
    alien.alien_box_y = 15;
    alien.display_start_index = 0;
    alien.display_stop_index = 4;
    alien.direction = 0;

    // Initialize the Alien Attack Linked List with NULL
    for (j = 0; j < 3; j++)
        for (i = 0; i < 20; i++)
            alien_attacks[j][i] = 0;

    alien_attack_x_arbiter = 2;
    alien_attack_y_arbiter = 0;
    alien_attack_delay = 0;

    for (j = 0; j < 3; j++)
        for (i = 0; i < 5; i++)
            alien_alive_status[j][i] = 1;

    // Initialise timer (load value, prescaler value, mode value)
    timer_init((Timer_Load_Value_For_One_Sec / 10), Timer_Prescaler, 1);
    timer_enable();

    pause = 0;

    // Print instructions
    printf("\n\n-------- EDK Demo ---------");
    printf("\n- Galactic Defenders Game -");
    printf("\nCentre btn ..... hard reset");
    printf("\nKeyboard r ..... soft reset");
    printf("\nKeyboard w ........ move up");
    printf("\nKeyboard s ...... move down");
    printf("\nKeyboard a ...... move left");
    printf("\nKeyboard d ..... move right");
    printf("\nKeyboard space ...... pause");
    printf("\n---------------------------");
    printf("\nTo ran the game, make sure:");
    printf("\n*UART terminal is activated");
    printf("\n*UART baud rare:  19200 bps");
    printf("\n*Keyboard is in lower case");
    printf("\n---------------------------");
    printf("\nPress any key to start\n");
    while (KBHIT() == 0);

  

    // VGA_plot_pixel (10,50,BLUE);
    DisplayScore(score, SCORE_X_START, SCORE_Y_START); // display score on image region
    NVIC_EnableIRQ(Timer_IRQn); // start timing
    NVIC_EnableIRQ(UART_IRQn);
}

void Game_Close(void) {
    clear_screen();
    score = 0;
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // flush screen
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    NVIC_DisableIRQ(Timer_IRQn);
    NVIC_DisableIRQ(UART_IRQn);
}

int GameOver(int win) {
    char key;

    NVIC_DisableIRQ(UART_IRQn);
    NVIC_DisableIRQ(Timer_IRQn);

    if (win) {
        printf("\nYou won the game!\n");
    } else {
        printf("\nGame over. You lose. Try again.\n");
    }

    printf("\nYour score: %d\n", score);
    printf("\nPress 'q' to quit");
    printf("\nPress 'r' to replay");

    while (1) {
        while (KBHIT() == 0);
        key = UartGetc();
        if (key == 'r') {
            return 1;
        } else if (key == 'q') {
            return 0;
        } else {
            printf("\nInvalid input");
        }
    }
}

void DisplayLives(void) {
    int i, j, k;
    int start_x = 5; // Starting x position for the first life icon
    int start_y = top_boundary + 5; // Position just above the bottom boundary

    // Clear previous lives display
    for (i = 0; i < 3; i++) { // Assuming max 3 lives
        for (j = 0; j < 5; j++) {
            for (k = 0; k < 5; k++) {
                VGA_plot_pixel(start_x + i * 6 + k, start_y + j, BLACK);
            }
        }
    }

    // Display current lives
    for (i = 0; i < rocket.lives; i++) {
        for (j = 0; j < 5; j++) {
            for (k = 0; k < 5; k++) {
                VGA_plot_pixel(start_x + i * 6 + k, start_y + j, small_life_icon[j][k]);
            }
        }
    }
}

void UpdateRocketDirections(void) {
    int left, right, shoot;
    int gpio_read;

    gpio_read = GPIO_read();

    left = gpio_read & 0x1;
    right = gpio_read & 0x2;
    shoot = gpio_read & 0x4;

    if (right)
        rocket.direction = 1;

    if (left)
        rocket.direction = 2;

    if (rocket_attack_button_released == 0 && shoot) {
        rocket_attack_button_released = 1;
        rocket.shoot = 1;
    } else if (rocket_attack_button_released == 1 && shoot == 0)
        rocket_attack_button_released = 0;
}

void DisplayRocket(void) {
    int i = 0, j = 0;

    for (j = 0; j <= 7; j++) {
        for (i = 0; i <= 10; i++) {
            if (i == 0 || i == 10)
                VGA_plot_pixel(rocket.x + i - 1, rocket.y + j, BLACK);
            else
                VGA_plot_pixel(rocket.x + i - 1, rocket.y + j, rocket_image[j][i - 1]);
        }
    }
}

// Function to update the Rocket coordinates
void MoveRocket(void) {
    if (rocket.direction == 1 && rocket.x < 89)
        rocket.x += 1;
    else if (rocket.direction == 2 && rocket.x > 2)
        rocket.x -= 1;

    rocket.direction = 0;
}

void GenerateRocketAttack(void) {
    int i;

    if (rocket.shoot == 1) {
        for (i = 0; i < 20; i++) {
            if (rocket_attacks[2][i] == 0) {
                rocket_attacks[0][i] = rocket.x + 4;
                rocket_attacks[1][i] = rocket.y;
                rocket_attacks[2][i] = 1;
                break;
            }
        }
    }

    rocket.shoot = 0;
}

int CheckTopCollision(int x, int y) {
    int i, j, k, l;

    for (i = 0; i <= 2; i++) {
        for (j = 0; j <= 4; j++) {
            if (alien_alive_status[i][j] == 1 && ((x >= alien.alien_box_x + j * 18) && (x <= alien.alien_box_x + j * 18 + 8) && (y >= alien.alien_box_y + i * 14) && (y <= alien.alien_box_y + i * 18 + 8) && (alien_image[y - alien.alien_box_y - i * 14][x - alien.alien_box_x - j * 18] != BLACK))) {
                alien_alive_status[i][j] = 0;

                for (k = 0; k < 9; k++)
                    for (l = 0; l < 9; l++)
                        VGA_plot_pixel(alien.alien_box_x + j * 18 + k, alien.alien_box_y + i * 14 + l, BLACK);

                VGA_plot_pixel(x, y, BLACK);

                // Increment score when alien is hit
                score++;
                DisplayScore(score, SCORE_X_START, SCORE_Y_START); // Update the displayed score

                return 1;
            }
        }
    }

    for (i = 0; i < 20; i++) {
        if (alien_attacks[2][i] == 1 && alien_attacks[0][i] == x && alien_attacks[1][i] == y - 1) {
            VGA_plot_pixel(x, y, BLACK);
            VGA_plot_pixel(x, y - 1, BLACK);
            alien_attacks[0][i] = 0;
            alien_attacks[1][i] = 0;
            alien_attacks[2][i] = 0;
            return 1;
        }
    }

    return 0;
}

void DisplayRocketAttacks(void) {
    int i;

    for (i = 0; i < 20; i++) {
        if (rocket_attacks[2][i] == 1) {
            if (CheckTopCollision(rocket_attacks[0][i], rocket_attacks[1][i]) == 1) {
                rocket_attacks[0][i] = 0;
                rocket_attacks[1][i] = 0;
                rocket_attacks[2][i] = 0;
            } else if (rocket_attacks[1][i] == 2) {
                VGA_plot_pixel(rocket_attacks[0][i], rocket_attacks[1][i], BLACK);
                rocket_attacks[0][i] = 0;
                rocket_attacks[1][i] = 0;
                rocket_attacks[2][i] = 0;
            } else {
                VGA_plot_pixel(rocket_attacks[0][i], rocket_attacks[1][i], BLACK);
                rocket_attacks[1][i] -= 1;
                VGA_plot_pixel(rocket_attacks[0][i], rocket_attacks[1][i], WHITE);
            }
        }
    }
}

// Function to display an Alien
void DisplayAlien(int x, int y) {
    int i = 0, j = 0;

    for (j = 0; j <= 8; j++) {
        for (i = 0; i <= 10; i++) {
            if (i == 0 || i == 10)
                VGA_plot_pixel(x + i - 1, y + j, BLACK);
            else
                VGA_plot_pixel(x + i - 1, y + j, alien_image[j][i - 1]);
        }
    }
}

// Function to update and display positions of all aliens
void UpdateAlienPositions(void) {
    int i, j;

    if (alien_alive_status[0][0] == 0 && alien_alive_status[1][0] == 0 && alien_alive_status[2][0] == 0) {
        alien.display_start_index = 1;
        if (alien_alive_status[0][1] == 0 && alien_alive_status[1][1] == 0 && alien_alive_status[2][1] == 0) {
            alien.display_start_index = 2;
            if (alien_alive_status[0][2] == 0 && alien_alive_status[1][2] == 0 && alien_alive_status[2][2] == 0) {
                alien.display_start_index = 3;
                if (alien_alive_status[0][3] == 0 && alien_alive_status[1][3] == 0 && alien_alive_status[2][3] == 0) {
                    alien.display_start_index = 4;
                    if (alien_alive_status[0][4] == 0 && alien_alive_status[1][4] == 0 && alien_alive_status[2][4] == 0) {
                        alien.display_start_index = 5;
                    }
                }
            }
        }
    }

    if (alien_alive_status[0][4] == 0 && alien_alive_status[1][4] == 0 && alien_alive_status[2][4] == 0) {
        alien.display_stop_index = 3;
        if (alien_alive_status[0][3] == 0 && alien_alive_status[1][3] == 0 && alien_alive_status[2][3] == 0) {
            alien.display_stop_index = 2;
            if (alien_alive_status[0][2] == 0 && alien_alive_status[1][2] == 0 && alien_alive_status[2][2] == 0) {
                alien.display_stop_index = 1;
                if (alien_alive_status[0][1] == 0 && alien_alive_status[1][1] == 0 && alien_alive_status[2][1] == 0) {
                    alien.display_stop_index = 0;
                    if (alien_alive_status[0][0] == 0 && alien_alive_status[1][0] == 0 && alien_alive_status[2][0] == 0) {
                        alien.display_stop_index = 0;
                    }
                }
            }
        }
    }

    if (alien.direction == 1 && (alien.alien_box_x + alien.display_stop_index * 18 + 8) == 97)
        alien.direction = 0;
    else if (alien.direction == 0 && (alien.alien_box_x + alien.display_start_index * 18) == 2)
        alien.direction = 1;

    if (alien.direction == 1)
        alien.alien_box_x += 1;
    else
        alien.alien_box_x -= 1;

    for (j = 0; j <= 2; j++) {
        for (i = 0; i <= 4; i++) {
            if (alien_alive_status[j][i] == 1) {
                DisplayAlien(alien.alien_box_x + i * 18, alien.alien_box_y + j * 14);
            }
        }
    }
}

// Function to generate the position of new Alien Attack
void GenerateAlienAttack(void) {
    int i;

    if (alien_attack_x_arbiter == 4)
        alien_attack_x_arbiter = 0;
    else
        alien_attack_x_arbiter += 1;

    while (alien_alive_status[0][alien_attack_x_arbiter] == 0 && alien_alive_status[1][alien_attack_x_arbiter] == 0 && alien_alive_status[2][alien_attack_x_arbiter] == 0) {
        if (alien_attack_x_arbiter == 4)
            alien_attack_x_arbiter = 0;
        else
            alien_attack_x_arbiter += 1;
    }

    if (alien_attack_y_arbiter == 2)
        alien_attack_y_arbiter = 0;
    else
        alien_attack_y_arbiter += 1;

    while (alien_alive_status[alien_attack_y_arbiter][alien_attack_x_arbiter] == 0) {
        if (alien_attack_y_arbiter == 2)
            alien_attack_y_arbiter = 0;
        else
            alien_attack_y_arbiter += 1;
    }

    for (i = 0; i < 20; i++) {
        if (alien_attacks[2][i] == 0) {
            alien_attacks[0][i] = alien.alien_box_x + (alien_attack_x_arbiter * 18) + 4;
            alien_attacks[1][i] = alien.alien_box_y + (alien_attack_y_arbiter * 14) + 8;
            alien_attacks[2][i] = 1;
            break;
        }
    }
}

void BlinkRocket(void) {
    int i, j, k, d;

    for (i = 0; i < 3; i++) {
        // Turn off the rocket
        for (j = 0; j <= 7; j++) {
            for (k = 0; k <= 8; k++) {
                VGA_plot_pixel(rocket.x + k, rocket.y + j, BLACK);
            }
        }
        // Delay
        for (d = 0; d < 150000; d++);

        // Turn on the rocket
        DisplayRocket();
        // Delay
        for (d = 0; d < 150000; d++);
    }
}

void DisplayAlienAttacks(void) {
    int i;

    for (i = 0; i < 20; i++) {
        if (alien_attacks[2][i] == 1) {
            if (alien_attacks[1][i] == 117 ||
                (alien_attacks[0][i] >= rocket.x && alien_attacks[0][i] <= rocket.x + 8 &&
                 alien_attacks[1][i] >= rocket.y && alien_attacks[1][i] <= rocket.y + 7 &&
                 rocket_image[alien_attacks[1][i] - rocket.y][alien_attacks[0][i] - rocket.x] != BLACK)) {

                // Collision detected between alien bullet and rocket
                if (alien_attacks[0][i] >= rocket.x && alien_attacks[0][i] <= rocket.x + 8 &&
                    alien_attacks[1][i] >= rocket.y && alien_attacks[1][i] <= rocket.y + 7 &&
                    rocket_image[alien_attacks[1][i] - rocket.y][alien_attacks[0][i] - rocket.x] != BLACK) {

                    // Decrement rocket's lives
                    rocket.lives--;

                    if (rocket.lives > 0) {
                        // Blink effect
                        BlinkRocket();
                    } else {
                        // End the game
                        int game_status = GameOver(0); // 0 for lose
                        if (game_status == 0) {
                            Game_Close(); // Exit the game
                        } else if (game_status == 1) {
                            Game_Close();
                            Game_Init(); // Restart the game
                        }
                    }
                }

                // Clear the bullet
                VGA_plot_pixel(alien_attacks[0][i], alien_attacks[1][i], BLACK);
                alien_attacks[0][i] = 0;
                alien_attacks[1][i] = 0;
                alien_attacks[2][i] = 0;
            } else {
                // Move the bullet
                VGA_plot_pixel(alien_attacks[0][i], alien_attacks[1][i], BLACK);
                alien_attacks[1][i] += 1;
                VGA_plot_pixel(alien_attacks[0][i], alien_attacks[1][i], RED);
            }
        }
    }

    // Display the number of lives remaining below the spaceship
    DisplayLives();
}

int CheckAllAliensHit(void) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 5; j++) {
            if (alien_alive_status[i][j] == 1) {
                return 0;
            }
        }
    }
    return 1;
}

//---------------------------------------------
// UART ISR -- used to input commands
//---------------------------------------------

void UART_ISR(void) {
    key = UartGetc();

    // Only update the rocket attack variable if the previous attack is finished
    if (rocket_has_attacked == 1) {
        if (key == SHOOT)
            rocket.shoot = 1;
    }

    // Only update the direction if the previous movement is finished
    if (rocket_has_moved == 1) {
        if (key == RIGHT)
            rocket.direction = 1;
        else if (key == LEFT)
            rocket.direction = 2;
    }
    if (key == PAUSE) {
        if (pause == 0) {
            pause = 1;
            NVIC_DisableIRQ(Timer_IRQn);
        } else {
            pause = 0;
            NVIC_EnableIRQ(Timer_IRQn);
        }
    }

    rocket_has_moved = 0;
    rocket_has_attacked = 0;
}

//---------------------------------------------
// TIMER ISR -- used to move the rocket
//---------------------------------------------

void Timer_ISR(void) {
    // If game is not paused
    if (pause == 0) {
        // Update the directions of rocket
        UpdateRocketDirections();

        // Update positions of aliens
        UpdateAlienPositions();

        // Shift the rocket
        MoveRocket();
        GenerateRocketAttack();
        DisplayRocketAttacks();
        DisplayRocket();

        if (alien_attack_delay == 10) {
            alien_attack_delay = 1;
            GenerateAlienAttack();
        } else
            alien_attack_delay += 1;

        DisplayAlienAttacks();

        // Check if all aliens are hit
        if (CheckAllAliensHit()) {
            int game_status = GameOver(1); // 1 for win
            if (game_status == 0) {
                Game_Close(); // Exit the game
            } else if (game_status == 1) {
                Game_Close();
                Game_Init(); // Restart the game
            }
        }
    }

    // Mark that rocket has moved
    rocket_has_moved = 1;
    rocket_has_attacked = 1;

    // Clear timer irq
    timer_irq_clear();
}

//---------------------------------------------
// Main Function
//---------------------------------------------

int main(void) {
    // Initialise the system
    SoC_init();
    // Initialise the game
    Game_Init();

    // Go to sleep mode and wait for interrupts
    while (1)
        __WFI();
}


