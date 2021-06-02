import pygame

WIDTH, HEIGHT = 600, 600
ROWS, COLS = 16, 16
GAME_ROWS,GAME_COLS = 13,13
BANK_ROW = 6
SQUARE_SIZE = WIDTH//COLS

# rgb
BROWN = (255, 153, 51)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BLUE = (0, 153, 255)
RED = (255,0,0)

#points
POINTS = {

    'a' : 2, 'b' : 4, 'c' : 3, 'd': 4, 'e' : 1, 'f' : 5, 'g' : 3, 'h' : 1, 'i' : 1,
    'j' : 5, 'k' : 3, 'l' : 3, 'm' : 3, 'n' : 3, 'o' : 2, 'p' : 4, 'q' : 5, 'r' : 3, 's' : 2, 't' : 3,
    'u' : 2, 'v' : 5, 'w' : 3, 'x' : 6, 'y' : 4, 'z' : 5
}

START_TOKENS = 7
START_BANK_SIZE = 50
MAX_TOKENS = 9
VOWEL_FACTOR = 18