import pygame
import enchant
from .constants import *
from pyScrabbles.board import Board
from .player import Player
from .Token import Token
en_dict = enchant.Dict("en_US")



class Game:
    def __init__(self, win):

        self.win = win
        self.curr_row = -1
        self.curr_col = -1
        self.player1 = Player(BLUE)
        self.player2 = Player(WHITE)
        self.bank = START_BANK_SIZE
        self.errors = ""
        self.winner_player = 0
        self._init()

    def drawSelected(self):
        if(self.curr_row != -1 and self.curr_col != -1):
            pygame.draw.rect(self.win, BLACK, (self.curr_col*SQUARE_SIZE, self.curr_row*SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))


    def update(self):
        self.board.draw(self.win)
        self.drawScores()
        self.draw_valid_moves(self.valid_moves)
        self.drawErrorMessage(self.errors)
        self.drawSelected()

        pygame.display.update()


    def drawErrorMessage(self, str):
        if(str == ""):
            return
        ls = str.split()
        pygame.font.init()
        font_scr = pygame.font.SysFont('Times New Roman', 20)
        for i in range(0,len(ls)):
            textsurface = font_scr.render(ls[i], False, RED)
            self.win.blit(textsurface, (ROWS * COLS + 270, ROWS * COLS +100+20*i))




    def drawScores(self):
        # draw current scores
        pygame.draw.rect(self.win, BLACK, (ROWS * COLS + 265, ROWS * COLS - 260, SQUARE_SIZE * 2 + 20, HEIGHT))
        pygame.font.init()
        font_scr = pygame.font.SysFont('Times New Roman', 15)

        #blue player
        textsurface = font_scr.render("Blue Points", False, BLUE)
        self.win.blit(textsurface, (ROWS * COLS + 265, ROWS * COLS - 255))
        textsurface = font_scr.render(str(self.player1.points), False, BLUE)
        self.win.blit(textsurface, (ROWS * COLS + 295, ROWS * COLS - 230))

        #white player
        textsurface = font_scr.render("White Points", False, WHITE)
        self.win.blit(textsurface, (ROWS * COLS + 265, ROWS * COLS - 200))
        textsurface = font_scr.render(str(self.player2.points), False, WHITE)
        self.win.blit(textsurface, (ROWS * COLS + 295, ROWS * COLS - 175))


        #current turn
        textsurface = font_scr.render("Current turn:", False, self.turn)
        self.win.blit(textsurface, (ROWS * COLS + 265, ROWS * COLS - 140))
        turn_string = "White"
        if(self.turn == BLUE):
            turn_string = "Blue"

        textsurface = font_scr.render(turn_string, False, self.turn)
        self.win.blit(textsurface, (ROWS * COLS + 285, ROWS * COLS - 120))

        #draw bank_button
        pygame.draw.rect(self.win, BROWN, (ROWS * COLS + 280, ROWS * COLS - 30, (SQUARE_SIZE * 2 + 10)//2, HEIGHT//16))
        pygame.draw.rect(self.win, self.turn,(ROWS * COLS + 282, ROWS * COLS - 28, (SQUARE_SIZE * 2 + 10) // 2 -4, HEIGHT //16-4))
        textsurface = font_scr.render("Bank", False, BLACK)
        self.win.blit(textsurface, (ROWS * COLS + 285, ROWS * COLS - 20))

        #draw bank remainder
        textsurface = font_scr.render("Bank", False, BROWN)
        self.win.blit(textsurface, (ROWS * COLS + 270, ROWS * COLS + 20))
        textsurface = font_scr.render("Remainder:", False, BROWN)
        self.win.blit(textsurface, (ROWS * COLS + 270, ROWS * COLS + 40))
        textsurface = font_scr.render(str(self.bank), False, BROWN)
        self.win.blit(textsurface, (ROWS * COLS + 270, ROWS * COLS +60))

        #draw tokens
        pygame.draw.rect(self.win, BLACK, ( ROWS * COLS - 255, ROWS * COLS + 265, WIDTH, SQUARE_SIZE * 2 + 20))
        pygame.draw.rect(self.win, WHITE, (7 * SQUARE_SIZE, 0, 2, HEIGHT))
        pygame.draw.rect(self.win, WHITE, (14 * SQUARE_SIZE, 0, 2, HEIGHT))

        font_scr = pygame.font.SysFont('Comic Sans MS', 15)

        textsurface = font_scr.render("Player 1", False, BLUE)
        self.win.blit(textsurface, (3 * SQUARE_SIZE, SQUARE_SIZE*14))
        textsurface = font_scr.render((str)(self.player1.tokens), False, BLUE)
        self.win.blit(textsurface, (1, SQUARE_SIZE * 15))

        textsurface = font_scr.render("Player 2", False, WHITE)
        self.win.blit(textsurface, (10 * SQUARE_SIZE, SQUARE_SIZE * 14))
        textsurface = font_scr.render((str)(self.player2.tokens), False, WHITE)
        self.win.blit(textsurface, (2+7*SQUARE_SIZE, SQUARE_SIZE * 15))






    def _init(self):
        self.selected = None
        self.board = Board()
        self.turn = WHITE
        self.curr_player = self.player2
        self.valid_moves = {}


    def reset(self):
        self._init()

    def select(self, row, col):
        if(row > GAME_ROWS or col> GAME_COLS):
            if(row == BANK_ROW and col > GAME_COLS):
                if(len(self.curr_player.tokens) >= MAX_TOKENS):
                    self.errors = "Max tokens reached"
                    return
                self.curr_player.tokens.append(Player.genTokens(1)[0])
                self.bank -= 1
                self.change_turn()
            return
        if(self.board.get_square(row,col) != 0):
            self.errors = "Location taken"
            return

        valid_moves = self.board.get_moves(row,col,self.turn)
        self.draw_valid_moves(valid_moves)
        self.curr_col = col
        self.curr_row = row




    def _place(self, row, col, char):
        if(self.curr_player.tokens.count(char) == 0):
            self.errors = "You do not have entered token"
            return
        token = Token(row, col, self.turn, char)
        self.curr_player.tokens.remove(char)
        self.board.place(token, row, col)
        emplace = self.spanStrings(row,col,self.turn)
        if len(emplace[0]) > 1 and en_dict.check(emplace[0]):
            self.curr_player.points += emplace[1]
        if len(emplace[2]) > 1 and en_dict.check(emplace[2]):
            self.curr_player.points += emplace[3]
        self.change_turn()


    def spanStrings(self,row,col,color):
        #return a quadruplet of values - [ver string, ver score, hor string, hor score]

        up_word_start = 0
        down_word_start = 0
        row_cpy = row
        while(self.board.legal_pos(row_cpy,col) and self.board.get_square(row_cpy,col) !=0):
            down_word_start = row_cpy
            row_cpy +=1
        row_cpy = row
        while (self.board.legal_pos(row_cpy, col) and self.board.get_square(row_cpy, col) != 0):
            up_word_start = row_cpy
            row_cpy -= 1
        ver_string = ""
        ver_score = 0
        for i in range(up_word_start,down_word_start+1):
            ver_string += self.board.get_square(i, col).char
            if (self.board.get_square(i, col).color == color):
                ver_score += POINTS[self.board.get_square(i, col).char]

        right_word_start = 0
        left_word_start = 0
        col_cpy = col
        while (self.board.legal_pos(row, col_cpy) and self.board.get_square(row, col_cpy) != 0):
            right_word_start = col_cpy
            col_cpy += 1
        col_cpy = col
        while (self.board.legal_pos(row, col_cpy) and self.board.get_square(row, col_cpy) != 0):
            left_word_start = col_cpy
            col_cpy -= 1
        hor_string = ""
        hor_score = 0
        for i in range(left_word_start, right_word_start+1):
            hor_string += self.board.get_square(row, i).char
            if(self.board.get_square(row,i).color == color):
                hor_score += POINTS[self.board.get_square(row, i).char]


        return [ver_string,ver_score,hor_string,hor_score]








    def draw_valid_moves(self, moves):
        for move in moves:
            row, col = move
            pygame.draw.circle(self.win, self.turn, (col * SQUARE_SIZE + SQUARE_SIZE//2, row * SQUARE_SIZE + SQUARE_SIZE//2), 15)

    def change_turn(self):
        self.valid_moves = {}
        self.curr_row  =-1
        self.curr_col = -1
        if self.turn == BLUE:
            self.turn = WHITE
            self.curr_player = self.player2
        else:
            self.turn = BLUE
            self.curr_player = self.player1

    def winner(self):
        if(self.bank == 0 or self.board.free_spots == 0):
            if (self.player1.points > self.player2.points):
                self.winner_player = 1
                return self.player1.color
            self.winner_player = 2
            return self.player2.color

        return None
    def drawWin(self):
        self.win.fill(WHITE)
        pygame.font.init()
        font_scr = pygame.font.SysFont('Times New Roman', 60)

        textsurface = font_scr.render("Winner:", False, BLACK)
        self.win.blit(textsurface, (150, 150))
        font_scr = pygame.font.SysFont('Comic Sans MS', 80)
        textsurface = font_scr.render("Player " + (str)(self.winner_player), False, BLACK)
        self.win.blit(textsurface, (200, 250))

        font_scr = pygame.font.SysFont('Times New Roman', 20)
        textsurface = font_scr.render("Press any key to leave", False, BLACK)
        self.win.blit(textsurface, (150, 500))

        pygame.display.update()



