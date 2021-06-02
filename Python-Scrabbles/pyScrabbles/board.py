import pygame
from .player import Player
from .constants import BLACK, ROWS, BROWN, SQUARE_SIZE, COLS, WHITE, BLUE, HEIGHT,WIDTH, GAME_ROWS, GAME_COLS
from .Token import Token

class Board:
    def __init__(self):
        self.board = []
        self.create_board()
        self.free_spots = ROWS*COLS
    
    def draw_squares(self, win):
        win.fill(BROWN)
        for row in range(ROWS):
            pygame.draw.rect(win, WHITE, (row * SQUARE_SIZE, -row, 2, HEIGHT))
            pygame.draw.rect(win, WHITE, (-row, row * SQUARE_SIZE, WIDTH, 2))






    def place(self, token, row, col):
            print((row))
            print(col)
            self.board[token.row][token.col], self.board[row][col] = self.board[row][col], token
            self.free_spots -= 1


    def get_square(self, row, col):
        return self.board[row][col]

    def create_board(self):
        for row in range(ROWS):
            self.board.append([])
            for col in range(COLS):
                self.board[row].append(0)


        
    def draw(self, win):
        self.draw_squares(win)
        for row in range(ROWS):
            for col in range(COLS):
                token = self.board[row][col]
                if token != 0:
                    token.draw(win)


    


    def get_moves(self, row, col, color):
        moves = []
        col_copy = col
        row_copy = row

        while(self.legal_pos(row,col_copy,color)):
            moves.append([row,col_copy])
            col_copy -= 1
        col_copy = col
        while (self.legal_pos(row, col_copy, color)):
            moves.append([row, col_copy])
            col_copy += 1
        while (self.legal_pos(row_copy, col, color)):
            moves.append([row_copy, col])
            row_copy -= 1
        row_copy = row
        while (self.legal_pos(row_copy,col,color)):
            moves.append([row_copy, col])
            row_copy += 1
        return moves



    def legal_pos(self,row,col,color = (-1,-1,-1)):
        if(color == (-1,-1,-1)):
            color = self.get_square(row,col)
        return (row>=0 and row <= GAME_ROWS and col>=0 and col<= GAME_COLS and (self.get_square(row,col) ==0 or self.get_square(row,col) == color))