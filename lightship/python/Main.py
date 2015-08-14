import lightship


class Game(lightship.Game):
    def __init__(self):
        print(self.get_name())


if __name__ == '__main__':
    lightship.register_game(Game())
