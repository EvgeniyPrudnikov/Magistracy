numbers = ['One', 'Two', 'Three', 'Four', 'Five', 'Six', 'Seven', 'Eight', 'Nine', 'Ten']
gb = ' green bottles '
hotw = 'hanging on the wall,'
ai = 'And if '
twb = "There'll be "
saf = ' should accidentally fall,'
th = 'that'
nm = 'no more'

res = list( num + gb + hotw if j in (0, 1)
            else ai + numbers[0].lower() + gb[:-2] + saf if j == 2 and i != len(numbers) - 1
                else ai + th + gb[:-2] + saf if j == 2 and i == len(numbers) - 1
                    else twb + numbers[::-1][i + 1].lower() + gb + hotw[:-1] + '.' + '\n' if i != len(numbers) - 1
                        else twb + nm + gb[-9::] + hotw[:-1] + '.'
            for i, num in enumerate(numbers[::-1]) for j in xrange(4))

print "\n".join(res)

'''
Ten green bottles hanging on the wall,
Ten green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be nine green bottles hanging on the wall.
...
Nine green bottles hanging on the wall,
Nine green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be eight green bottles hanging on the wall.
...
Eight green bottles hanging on the wall,
Eight green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be seven green bottles hanging on the wall.
...
Seven green bottles hanging on the wall,
Seven green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be six green bottles hanging on the wall.
...
Six green bottles hanging on the wall,
Six green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be five green bottles hanging on the wall.
...
Five green bottles hanging on the wall,
five green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be four green bottles hanging on the wall.
...
Four green bottles hanging on the wall,
four green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be three green bottles hanging on the wall.
...
Three green bottles hanging on the wall,
Three green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be two green bottles hanging on the wall.
...
Two green bottles hanging on the wall,
Two green bottles hanging on the wall.
And if one green bottle should accidentally fall,
There'll be one green bottle hanging on the wall.
...
One green bottle hanging on the wall,
One green bottle hanging on the wall.
And if that green bottle should accidentally fall,
There'll be no more bottles hanging on the wall.
'''