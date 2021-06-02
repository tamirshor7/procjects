from .constants import BROWN, WHITE, SQUARE_SIZE, BLACK, POINTS
import pygame


class Token:
    PADDING = 5
    OUTLINE = 2

    def __init__(self, row, col, color, char):
        self.row = row
        self.col = col
        self.color = color
        self.x = 0
        self.y = 0
        self.char = char
        self.points = POINTS[char]
        self.calc_pos()

    def calc_pos(self):
        self.x = SQUARE_SIZE * self.col + SQUARE_SIZE // 2
        self.y = SQUARE_SIZE * self.row + SQUARE_SIZE // 2

    def draw(self, win):
        radius = SQUARE_SIZE // 2 - self.PADDING

        pygame.draw.circle(win, BLACK, (self.x, self.y), radius + self.OUTLINE)
        pygame.draw.circle(win, self.color, (self.x, self.y), radius)

        pygame.font.init()
        font_char = pygame.font.SysFont('Comic Sans MS', 15)
        textsurface = font_char.render(self.char, False, BLACK)
        win.blit(textsurface, (self.x - 4, self.y - 17))

        font_points = pygame.font.SysFont('Comic Sans MS', 10)
        textsurface = font_points.render((str)(self.points), False, BLACK)
        win.blit(textsurface, (self.x, self.y - 1))



    def __repr__(self):
        return str(self.color)