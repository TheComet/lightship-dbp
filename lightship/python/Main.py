import lightship
from lightship import LightshipError


class Game(lightship.Game):

	def __init__(self):
		print('game name: "{}"'.format(self.get_name()))
		try:
			print('network role: "{}"'.format(self.get_network_role()))
		except LightshipError:
			print('Error')

	def __del__(self):
		print('oh shit')


if __name__ == '__main__':
	game = Game()
	lightship.register_game(game)
