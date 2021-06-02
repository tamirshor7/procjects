import random
from string import ascii_lowercase as asciis_low
from .constants import START_TOKENS, VOWEL_FACTOR


class Player:
    def __init__(self,color):
        self.color = color
        self.points = 0
        self.tokens = Player.genTokens(START_TOKENS)

    @staticmethod
    def genTokens(tok_num):
        tokens = []
        vowels = ['a','e','i','o','u']
        for i in range(0,tok_num):
            char = random.choice(asciis_low)
            j = VOWEL_FACTOR
            while(j>0 and vowels.count(char) == 0): #increase probability for a vowel
                char = random.choice(asciis_low)
                j -= 1
            tokens.append(random.choice(asciis_low))
        return tokens
