''' '''
'''
 ISC License

 Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

'''
import sys, os, inspect
import numpy as np
from Basilisk.utilities import unitTestSupport
import math

filename = inspect.getframeinfo(inspect.currentframe()).filename
path = os.path.dirname(os.path.abspath(filename))
splitPath = path.split('fswAlgorithms')




import matplotlib.pyplot as plt


def StateErrorCovarPlot(x, Pflat, show_plots):


    P = np.zeros([len(Pflat[:,0]),6,6])
    t= np.zeros(len(Pflat[:,0]))
    for i in range(len(Pflat[:,0])):
        t[i] = x[i, 0]*1E-9
        P[i,:,:] = Pflat[i,1:37].reshape([6,6])
        for j in range(len(P[0,0,:])):
            P[i,j,j] = np.sqrt(P[i,j,j])

    plt.figure(num=None, figsize=(10, 10), dpi=80, facecolor='w', edgecolor='k')
    plt.subplot(321)
    plt.plot(t , x[:, 1], "b", label='Error Filter')
    plt.plot(t , 3 * np.sqrt(P[:, 0, 0]), 'r--',  label='Covar Filter')
    plt.plot(t , -3 * np.sqrt(P[:, 0, 0]), 'r--')
    plt.legend(loc='lower right')
    plt.title('First LOS component')
    plt.grid()

    plt.subplot(322)
    plt.plot(t , x[:, 4], "b")
    plt.plot(t , 3 * np.sqrt(P[:, 3, 3]), 'r--')
    plt.plot(t , -3 * np.sqrt(P[:, 3, 3]), 'r--')
    plt.title('First rate component')
    plt.grid()

    plt.subplot(323)
    plt.plot(t , x[:, 2], "b")
    plt.plot(t , 3 * np.sqrt(P[:, 1, 1]), 'r--')
    plt.plot(t , -3 * np.sqrt(P[:, 1, 1]), 'r--')
    plt.title('Second LOS component')
    plt.grid()

    plt.subplot(324)
    plt.plot(t , x[:, 5], "b")
    plt.plot(t , 3 * np.sqrt(P[:, 4, 4]), 'r--')
    plt.plot(t , -3 * np.sqrt(P[:, 4, 4]), 'r--')
    plt.title('Second rate component')
    plt.grid()

    plt.subplot(325)
    plt.plot(t , x[:, 3], "b")
    plt.plot(t , 3 * np.sqrt(P[:, 2, 2]), 'r--')
    plt.plot(t , -3 * np.sqrt(P[:, 2, 2]), 'r--')
    plt.xlabel('t(s)')
    plt.title('Third LOS component')
    plt.grid()

    plt.subplot(326)
    plt.plot(t , x[:, 6], "b")
    plt.plot(t , 3 * np.sqrt(P[:, 5, 5]), 'r--')
    plt.plot(t , -3 * np.sqrt(P[:, 5, 5]), 'r--')
    plt.xlabel('t(s)')
    plt.title('Third rate component')
    plt.grid()

    unitTestSupport.writeFigureLaTeX('StatesPlot', 'State error and covariance', plt, 'height=0.9\\textwidth, keepaspectratio', path)
    if show_plots:
        plt.show()
    plt.close('all')


def StatesPlotCompare(x, x2, Pflat, Pflat2, show_plots):


    P = np.zeros([len(Pflat[:,0]),6,6])
    P2 = np.zeros([len(Pflat[:,0]),6,6])
    t= np.zeros(len(Pflat[:,0]))
    for i in range(len(Pflat[:,0])):
        t[i] = x[i, 0]*1E-9
        P[i,:,:] = Pflat[i,1:37].reshape([6,6])
        P2[i, :, :] = Pflat2[i, 1:37].reshape([6, 6])

    plt.figure(num=None, figsize=(10, 10), dpi=80, facecolor='w', edgecolor='k')
    plt.subplot(321)
    plt.plot(t[0:30] , x[0:30, 1], "b", label='Error Filter')
    plt.plot(t[0:30] , 3 * np.sqrt(P[0:30, 0, 0]), 'r--',  label='Covar Filter')
    plt.plot(t[0:30] , -3 * np.sqrt(P[0:30, 0, 0]), 'r--')
    plt.plot(t[0:30] , x2[0:30, 1], "g", label='Error Expected')
    plt.plot(t[0:30] , 3 * np.sqrt(P2[0:30, 0, 0]), 'c--', label='Covar Expected')
    plt.plot(t[0:30] , -3 * np.sqrt(P2[0:30, 0, 0]), 'c--')
    plt.legend(loc='lower right')
    plt.title('First LOS component')
    plt.grid()

    plt.subplot(322)
    plt.plot(t[0:30] , x[0:30, 4], "b")
    plt.plot(t[0:30] , 3 * np.sqrt(P[0:30, 3, 3]), 'r--')
    plt.plot(t[0:30] , -3 * np.sqrt(P[0:30, 3, 3]), 'r--')
    plt.plot(t[0:30] , x2[0:30, 4], "g")
    plt.plot(t[0:30] , 3 * np.sqrt(P2[0:30, 3, 3]), 'c--')
    plt.plot(t[0:30] , -3 * np.sqrt(P2[0:30, 3, 3]), 'c--')
    plt.title('First rate component')
    plt.grid()

    plt.subplot(323)
    plt.plot(t[0:30] , x[0:30, 2], "b")
    plt.plot(t[0:30] , 3 * np.sqrt(P[0:30, 1, 1]), 'r--')
    plt.plot(t[0:30] , -3 * np.sqrt(P[0:30, 1, 1]), 'r--')
    plt.plot(t[0:30] , x2[0:30, 2], "g")
    plt.plot(t[0:30] , 3 * np.sqrt(P2[0:30, 1, 1]), 'c--')
    plt.plot(t[0:30] , -3 * np.sqrt(P2[0:30, 1, 1]), 'c--')
    plt.title('Second LOS component')
    plt.grid()

    plt.subplot(324)
    plt.plot(t[0:30] , x[0:30, 5], "b")
    plt.plot(t[0:30] , 3 * np.sqrt(P[0:30, 4, 4]), 'r--')
    plt.plot(t[0:30] , -3 * np.sqrt(P[0:30, 4, 4]), 'r--')
    plt.plot(t[0:30] , x2[0:30, 5], "g")
    plt.plot(t[0:30] , 3 * np.sqrt(P2[0:30, 4, 4]), 'c--')
    plt.plot(t[0:30] , -3 * np.sqrt(P2[0:30, 4, 4]), 'c--')
    plt.title('Second rate component')
    plt.grid()

    plt.subplot(325)
    plt.plot(t[0:30] , x[0:30, 3], "b")
    plt.plot(t[0:30] , 3 * np.sqrt(P[0:30, 2, 2]), 'r--')
    plt.plot(t[0:30] , -3 * np.sqrt(P[0:30, 2, 2]), 'r--')
    plt.plot(t[0:30] , x2[0:30, 3], "g")
    plt.plot(t[0:30] , 3 * np.sqrt(P2[0:30, 2, 2]), 'c--')
    plt.plot(t[0:30] , -3 * np.sqrt(P2[0:30, 2, 2]), 'c--')
    plt.xlabel('t(s)')
    plt.title('Third LOS component')
    plt.grid()

    plt.subplot(326)
    plt.plot(t[0:30] , x[0:30, 6], "b")
    plt.plot(t[0:30] , 3 * np.sqrt(P[0:30, 5, 5]), 'r--')
    plt.plot(t[0:30] , -3 * np.sqrt(P[0:30, 5, 5]), 'r--')
    plt.plot(t[0:30] , x2[0:30, 6], "g")
    plt.plot(t[0:30] , 3 * np.sqrt(P2[0:30, 5, 5]), 'c--')
    plt.plot(t[0:30] , -3 * np.sqrt(P2[0:30, 5, 5]), 'c--')
    plt.xlabel('t(s)')
    plt.title('Third rate component')
    plt.grid()

    unitTestSupport.writeFigureLaTeX('StatesCompare', 'State error and covariance vs expected Values', plt, 'height=0.9\\textwidth, keepaspectratio', path)

    if show_plots:
        plt.show()
    plt.close()

def PostFitResiduals(Res, noise, show_plots):

    MeasNoise = np.zeros(len(Res[:,0]))
    t= np.zeros(len(Res[:,0]))
    for i in range(len(Res[:,0])):
        t[i] = Res[i, 0]*1E-9
        MeasNoise[i] = 3*noise
        # Don't plot zero values, since they mean that no measurement is taken
        for j in range(len(Res[0,:])-1):
            if -1E-10 < Res[i,j+1] < 1E-10:
                Res[i, j+1] = np.nan

    plt.figure(num=None, figsize=(10, 10), dpi=80, facecolor='w', edgecolor='k')
    plt.subplot(421)
    plt.plot(t , Res[:, 1], "b.", label='Residual')
    plt.plot(t , MeasNoise, 'r--', label='Covar')
    plt.plot(t , -MeasNoise, 'r--')
    plt.legend(loc='lower right')
    plt.ylim([-10*noise, 10*noise])
    plt.title('First CSS')
    plt.grid()

    plt.subplot(422)
    plt.plot(t , Res[:, 5], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.title('Fifth CSS')
    plt.grid()

    plt.subplot(423)
    plt.plot(t , Res[:, 2], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.title('Second CSS')
    plt.grid()

    plt.subplot(424)
    plt.plot(t , Res[:, 6], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.title('Sixth CSS')
    plt.grid()

    plt.subplot(425)
    plt.plot(t , Res[:, 3], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.title('Third CSS')
    plt.grid()

    plt.subplot(426)
    plt.plot(t , Res[:, 7], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.title('Seventh CSS')
    plt.grid()

    plt.subplot(427)
    plt.plot(t , Res[:, 4], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.xlabel('t(s)')
    plt.title('Fourth CSS')
    plt.grid()

    plt.subplot(428)
    plt.plot(t , Res[:, 8], "b.")
    plt.plot(t , MeasNoise, 'r--')
    plt.plot(t , -MeasNoise, 'r--')
    plt.ylim([-10*noise, 10*noise])
    plt.xlabel('t(s)')
    plt.title('Eight CSS')
    plt.grid()

    unitTestSupport.writeFigureLaTeX('PostFit', 'Post Fit Residuals', plt, 'height=0.9\\textwidth, keepaspectratio', path)

    if show_plots:
        plt.show()
    plt.close()

def StatesVsExpected(stateLog, expectedStateArray, show_plots):
    plt.figure(num=None, figsize=(10, 10), dpi=80, facecolor='w', edgecolor='k')
    plt.subplot(321)
    plt.plot(stateLog[:, 0] * 1.0E-9, expectedStateArray[:,  1], 'b--', label='Expected')
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:,  1], 'r', label='Filter')
    plt.legend(loc='lower right')
    plt.title('First LOS component')
    plt.grid()

    plt.subplot(323)
    plt.plot(stateLog[:, 0] * 1.0E-9, expectedStateArray[:,  2], 'b--')
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:,  2], 'r')
    plt.title('Second LOS component')
    plt.grid()

    plt.subplot(324)
    plt.plot(stateLog[:, 0] * 1.0E-9, expectedStateArray[:,  4], 'b--')
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:,  4], 'r')
    plt.title('Second rate component')
    plt.grid()

    plt.subplot(325)
    plt.plot(stateLog[:, 0] * 1.0E-9, expectedStateArray[:,  3], 'b--')
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:,  3], 'r')
    plt.xlabel('t(s)')
    plt.title('Third LOS component')
    plt.grid()

    plt.subplot(326)
    plt.plot(stateLog[:, 0] * 1.0E-9, expectedStateArray[:,  5], 'b--')
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:,  5], 'r')
    plt.xlabel('t(s)')
    plt.title('Third rate component')
    plt.grid()

    unitTestSupport.writeFigureLaTeX('StatesExpected', 'States vs true states in static case', plt, 'height=0.9\\textwidth, keepaspectratio', path)

    if show_plots:
        plt.show()
    plt.close()



def StatesVsTargets(target1, target2, stateLog, show_plots):


    target = np.ones([len(stateLog[:, 0]),6])
    for i in range((len(stateLog[:, 0])-1)//2):
        target[i, :] = target1
        target[i+(len(stateLog[:, 0]) - 1) // 2,:] = target2

    plt.figure(num=None, figsize=(10, 10), dpi=80, facecolor='w', edgecolor='k')
    plt.subplot(321)
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:, 1], 'b', label='Filter')
    plt.plot(stateLog[:, 0] * 1.0E-9, target[:, 0], 'r--', label='Expected')
    plt.legend(loc='lower right')
    plt.title('First LOS component')
    plt.grid()

    plt.subplot(322)
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:, 4], 'b')
    plt.plot(stateLog[:, 0] * 1.0E-9, target[:, 3], 'r--')
    plt.title('First rate component')
    plt.grid()

    plt.subplot(323)
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:, 2], 'b')
    plt.plot(stateLog[:, 0] * 1.0E-9, target[:, 1], 'r--')
    plt.title('Second LOS component')
    plt.grid()

    plt.subplot(324)
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:, 5], 'b')
    plt.plot(stateLog[:, 0] * 1.0E-9, target[:, 4], 'r--')
    plt.title('Second rate component')
    plt.grid()

    plt.subplot(325)
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:, 3], 'b')
    plt.plot(stateLog[:, 0] * 1.0E-9, target[:, 2], 'r--')
    plt.xlabel('t(s)')
    plt.title('Third LOS component')
    plt.grid()

    plt.subplot(326)
    plt.plot(stateLog[:, 0] * 1.0E-9, stateLog[:, 6], 'b')
    plt.plot(stateLog[:, 0] * 1.0E-9, target[:, 5], 'r--')
    plt.xlabel('t(s)')
    plt.title('Third rate component')
    plt.grid()

    unitTestSupport.writeFigureLaTeX('StatesTarget', 'States tracking target values', plt, 'height=0.9\\textwidth, keepaspectratio', path)

    if show_plots:
        plt.show()
    plt.close()
