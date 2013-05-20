/*
-----------------------------------------------------------------------------
This source file is part of Dflora (Dancing Flora) OpenGL|ES 1.1 version

Copyright (c) 2004-2005 Changzhi Li < richardzlee@163.com >

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free 
Software Foundation; either version 2 of the License, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
details.

You should have received a copy of the GNU Lesser General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 59 
Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#ifdef WIN32
#pragma warning( disable : 4514 4100 4505 )
#endif

#include "timer.h"

Timer::Timer()
{
	m_StartTime = 0;
	m_PauseTime = 0;
	m_bPaused = true;
}

void Timer::Start()
{
	m_StartTime = clock();
	m_bPaused = false;
}

void Timer::Stop()
{
	m_StartTime = 0;
}

void Timer::Pause()
{
	m_bPaused = true;
	m_PauseTime = clock();
}

void Timer::Resume()
{
	m_bPaused = false;
	m_StartTime += clock() - m_PauseTime;
}

TimeValue Timer::GetElapsedTime()
{
	long delta;
	if(m_bPaused)
		delta = m_PauseTime - m_StartTime;	
	else
		delta = clock() - m_StartTime;
	return delta*1000/CLOCKS_PER_SEC;	
}

Timer g_Timer;

bool Interval::Contain( const Interval & interval ) const	
{
	return Contain( interval.Start() ) && Contain( interval.End() ); 
}

bool Interval::StartIn( const Interval & interval ) const	
{
	return interval.Contain( Start() ); 
}

bool Interval::EndIn( const Interval & interval ) const
{ 
	return interval.Contain( End() ); 
}

Interval FOREVER(TIME_NegInfinity, TIME_PosInfinity);