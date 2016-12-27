import pygame
import remoteTask

red = (200, 0, 0)
green = (0, 200, 0)
white = (200, 200, 200)
black = (0,0,0)

bright_red = (255, 0, 0)
bright_green = (0, 255, 0)

pygame.init()

display_width = 800
display_height = 600

gameDisplay = pygame.display.set_mode((display_width,display_height))
pygame.display.set_caption('A bit Racey')

clock = pygame.time.Clock()

def text_objects(text, font):
    textSurface = font.render(text, True, black)
    return textSurface, textSurface.get_rect()

def button(msg, x, y, w, h, ic, ac, action=None):
    mouse = pygame.mouse.get_pos()
    click = pygame.mouse.get_pressed()
    print(click)
    if x + w > mouse[0] > x and y + h > mouse[1] > y:
        pygame.draw.rect(gameDisplay, ac, (x, y, w, h))

        if click[0] == 1 and action != None:
            action()
    else:
        pygame.draw.rect(gameDisplay, ic, (x, y, w, h))

    smallText = pygame.font.SysFont("comicsansms", 20)
    textSurf, textRect = text_objects(msg, smallText)
    textRect.center = ((x + (w / 2)), (y + (h / 2)))
    gameDisplay.blit(textSurf, textRect)


def game_intro():
    intro = True
    # remoteTask.getSocket()
    while intro:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        gameDisplay.fill(white)
        largeText = pygame.font.SysFont("comicsansms", 115)

        button("forward", 0, 0, 400, 100, green, bright_green, remoteTask.forward)
        button("left", 0, 100, 200, 100, green, bright_green, remoteTask.left)
        button("right", 200, 100, 200, 100, green, bright_green, remoteTask.right)
        button("backward", 0, 200, 400, 100, red, bright_red, remoteTask.backward)

        pygame.display.update()
        clock.tick(15)

remoteTask.getSocket()
game_intro()