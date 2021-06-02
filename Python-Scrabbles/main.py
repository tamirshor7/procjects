import pygame
from pyScrabbles.constants import WIDTH, HEIGHT, SQUARE_SIZE, BROWN
from pyScrabbles.game import Game

FPS = 60

WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption('Scrabbles')

def get_row_col_from_mouse(pos):
    x, y = pos
    row = y // SQUARE_SIZE
    col = x // SQUARE_SIZE
    return row, col

def main():
    run = True
    clock = pygame.time.Clock()
    game = Game(WIN)

    while run:
        clock.tick(FPS)

        if game.winner() != None:
            game.drawWin()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                if(game.winner_player != 0):
                    run = False
                game.errors = ""
                pos = pygame.mouse.get_pos()
                row, col = get_row_col_from_mouse(pos)
                game.select(row, col)
            if ((event.type == pygame.KEYDOWN)):
                if (game.winner_player != 0):
                    run = False
                if ((game.curr_row != -1) and (game.curr_col != 1)):
                    game.errors = ""
                    char = (chr)(pygame.key.get_pressed().index(1) + 93)
                    game._place(game.curr_row,game.curr_col, char)
        if(game.winner_player == 0):
            game.update()
    
    pygame.quit()

main()