import numpy as np
from PIL import Image
import math as m
import os
import cv2


def hash(p: tuple[float, float]) -> tuple[float, float]:
    p = (np.dot(p, (127.1, 311.7)), np.dot(p, (269.5, 183.3)))
    s = (m.sin(p[0]) * 43758.5453123, m.sin(p[1]) * 43758.5453123)
    return (s[0] - m.floor(s[0]), s[1] - m.floor(s[1]))

K1 = 0.366025404  # (sqrt(3)-1)/2
K2 = 0.211324865  # (3-sqrt(3))/6

def noise(p: tuple[float, float]) -> float:
    i = (m.floor(p[0] + (p[0] + p[1]) * K1), m.floor(p[1] + (p[0] + p[1]) * K1))
    a = (p[0] - (i[0] - (i[0] + i[1]) * K2), p[1] - (i[1] - (i[0] + i[1]) * K2))
    o = 0.0
    if a[0] + a[1] > 1.0:
        o = 1.0
    b = (a[0] - o + K2, a[1] - o + K2)
    c = (a[0] - 1.0 + 2.0 * K2, a[1] - 1.0 + 2.0 * K2)

    h = [hash((i[0], i[1])), hash((i[0] + o, i[1] + (1.0 - o))), hash((i[0] + 1.0, i[1] + 1.0))]

    w = [max(0.5 - a[0]*a[0] - a[1]*a[1], 0.0),
         max(0.5 - b[0]*b[0] - b[1]*b[1], 0.0),
         max(0.5 - c[0]*c[0] - c[1]*c[1], 0.0)]

    w = [w[i]**4 for i in range(3)]

    n = [w[i] * (h[i][0] * a[i % 3][0] + h[i][1] * a[i % 3][1]) for i in range(3)]

    return (n[0] + n[1] + n[2]) * 70.0

def fbm(p: tuple[float, float], time: float) -> float:
    c = m.cos(time / m.sqrt(3.0))
    d = noise(p)
    d += 0.5 * noise((p[0] + c, p[1]))
    d += 0.25 * noise((p[0], p[1] + c))
    d += 0.125 * noise((p[0] - c, p[1]))
    d /= (1.0 + 0.5 + 0.25 + 0.125 + 0.0625)
    return d

def toPolar(p: tuple[float, float]) -> tuple[float, float]:
    r = m.sqrt(p[0] * p[0] + p[1] * p[1])
    theta = m.atan2(p[1], p[0])
    return (r, theta)

def toRect(p: tuple[float, float]) -> tuple[float, float]:
    x = p[0] * m.cos(p[1])
    y = p[0] * m.sin(p[1])
    return (x, y)


thickness = 0.15
haze = 1.5
size = 0.075
count = 3

def electricNyanCat(uv: tuple[float, float], time: float) -> float:
    pp = toPolar(uv)
    pp[1] += 0.2 * p[0]
    p = toRect(pp)
    col:tuple[float, float, float] = (0.0, 0.0, 0.0)

    

