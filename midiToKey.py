#GARBAGE MIDI DETECTION
#DISCONTINUED (use other lib?)

import pygame
import pygame.midi

black = 0, 0, 0
white = 255, 255, 255
orange = 255, 128, 0
green = 64, 255, 0
iState = 0
iFontSize = 12
iTextOffset = 12
iMidiDebugTextOffset = 0
isExit = False

radius = 15
x = 0
y = 0
mode = 'blue'
data = []

pygame.init()
pygame.midi.init()
font = pygame.font.Font('freesansbold.ttf', iFontSize) 
screen = pygame.display.set_mode((640, 480))
clock = pygame.time.Clock()
    
deviceList = {}
deviceSelectionIndex = -1
#scan devices
for i in range(0, 255):
	info = pygame.midi.get_device_info(i)
	if info == None:
		break
	if info[2] == 0: # not an input device
		i += 1
		# continue
	# print("device: %d"%(i),info)
	deviceList[i]= "device: {} {}".format(i, info)
	if deviceSelectionIndex == -1:
		deviceSelectionIndex = i

def drawText(textIn, color = white):
	global iTextOffset 
	text = font.render(textIn, True, color)
	# textRect = text.get_rect()
	# textRect.move(0, 100)
	screen.blit(text, (0,iTextOffset)) 
	iTextOffset = iTextOffset + iFontSize

def drawMidiDebugText(textIn, color = white):
	global iTextOffset 
	text = font.render(textIn, True, color)
	# textRect = text.get_rect()
	# textRect.move(0, 100)
	screen.blit(text, (0,iTextOffset)) 
	iTextOffset = iTextOffset + iFontSize

def empty_Draw():
    drawText('nothing...')

def deviceSelection_Draw():
    global deviceSelectionIndex
    global deviceList
    drawText('Select midi device [0-255], Press ENTER to continue...')
    drawText('Selected Device: %d'%(deviceSelectionIndex), orange)

    for device in deviceList.keys():
        if deviceSelectionIndex == device:
            drawText(deviceList[device], orange)
        else:
            drawText(deviceList[device])

def deviceHandler_Draw():
    global deviceSelectionIndex
    device = pygame.midi.Input(deviceSelectionIndex)
    drawText('midi status: {}'.format(pygame.midi.get_init()))
    # for i in range(0, 255):
    #     info = pygame.midi.get_device_info(i)
    #     if not (info == None) or not (info == 0):
    #         drawText("{} {}".format(i, info))
    info = pygame.midi.get_device_info(deviceSelectionIndex)
    drawText('midi time: {}'.format(pygame.midi.time()))
    if not (info == None) or not (info == 0):
        drawText("{} {}".format(deviceSelectionIndex, info))
    hasData = device.poll()
    localData = device.read(1000)
    global data
    drawText('has data: {}, stored: {}, (unhandled {}) events: {}'.format(hasData, len(data), len(localData), localData))

    global iMidiDebugTextOffset
    iMidiDebugTextOffset = iMidiDebugTextOffset + iTextOffset
    
    if hasData:
        data.append(localData)
    for d in data:
        # print('midi message {} {}'.format(len(data), d))
        drawMidiDebugText("{}".format(d))

def empty_Keys():
    global isExit
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            isExit = True
            return
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                isExit = True
                return
def midi_Keys():
    global isExit
    for event in pygame.event.get():
        if event.type == pygame.midi.MIDIIN:
            print('midiIn: {}', event)
        if event.type == pygame.QUIT:
            isExit = True
            return
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                isExit = True
                return

def deviceSelection_Keys():
    global mode
    global radius
    global points
    global isExit
    global deviceSelectionIndex
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            isExit = True
            return
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w and ctrl_held:
                return
            if event.key == pygame.K_F4 and alt_held:
                return
            if event.key == pygame.K_ESCAPE:
                isExit = True
                return
            if event.key == pygame.K_RETURN:
                print('DEV_LDR')
                global iState
                iState = iState + 1
                return
            if event.key == pygame.K_LEFT:
                print('DEV_DEC')
                deviceSelectionIndex = deviceSelectionIndex - 1
                if(deviceSelectionIndex < 0):
                    deviceSelectionIndex = 255
            if event.key == pygame.K_RIGHT:
                print('DEV_INC')
                deviceSelectionIndex = deviceSelectionIndex + 1
                if(deviceSelectionIndex > 255):
                    deviceSelectionIndex = 0
        
            # # determine if a letter key was pressed
            # if event.key == pygame.K_r:
            #     mode = 'red'
            # elif event.key == pygame.K_g:
            #     mode = 'green'
            # elif event.key == pygame.K_b:
            #     mode = 'blue'
        
        # if event.type == pygame.MOUSEBUTTONDOWN:
        #     if event.button == 1: # left click grows radius
        #         radius = min(200, radius + 1)
        #     elif event.button == 3: # right click shrinks radius
        #         radius = max(1, radius - 1)
        
        # if event.type == pygame.MOUSEMOTION:
        #     # if mouse moved, add point to list
        #     position = event.pos
        #     points = points + [position]
        #     points = points[-256:]

def reset():
    global iTextOffset
    iTextOffset = 0

def main():
    while True:
        #clear
        reset()

        #key handling
        pressed = pygame.key.get_pressed()
        
        if iState == 0:
            deviceSelection_Keys()
        elif iState == 1:
            midi_Keys()
        else:
            empty_Keys()

        global isExit
        if isExit:
            return
                
        screen.fill((0, 0, 0))
        
        if iState == 0:
            deviceSelection_Draw()
        elif iState == 1:
            deviceHandler_Draw()
        else:
            empty_Draw()
        
        pygame.display.flip()
        
        clock.tick(60)

main()