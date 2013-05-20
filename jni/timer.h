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

#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>

#define TIME_PosInfinity	TimeValue(0x7fffffff)
#define TIME_NegInfinity 	TimeValue(0x80000000)

/** A uniform time unit, millisecond in this case. 
	@par
	This typedef is adapted from 3DS MAX's SDK to unify the time uint representaion on 
	the function calling interface.
	@see
	Interval
*/ 
typedef int TimeValue;

/** A Timer support start/end and pause/resume with the precision of millisecond.
	@par
	It is used as the time pump of the whole animation in this demo.
	@see 
	TimeValue, Interval
*/
class Timer
{
private:
	bool m_bPaused;
    TimeValue m_StartTime, m_PauseTime;	// msecond as its unit
	
public:
	Timer();
	void Start();
	void Stop();
	void Pause();
	void Resume();
	TimeValue GetElapsedTime();
};

/** Time interval of [s, e], close at both beginning and ending.
	@par
	This class is mainly borrowed from 3DS MAX's SDK, to conviently determine the 
	beginning, ending and duration of animation and rendering of all the objects in
	the scene.
	@see
	TimeValue
*/
class Interval
{
private:
	TimeValue	m_Start;
	TimeValue	m_End;

public:
	Interval()							{ SetEmpty(); }
	Interval(TimeValue s, TimeValue e)	{ Set(s, e); }
	~Interval()							{ }

	void SetEmpty()		{ m_Start = TIME_NegInfinity; m_End = TIME_NegInfinity; }
	void SetInfinite()	{ m_Start = TIME_NegInfinity; m_End = TIME_PosInfinity; }
	void SetInstant(const TimeValue t) { m_Start = m_End = t; }

	void Set       ( TimeValue s, TimeValue e ) { m_Start = s; m_End = e; }
	void SetStart  ( TimeValue s )              { m_Start = s;          }
	void SetEnd    ( TimeValue e )              { m_End = e;            }

	TimeValue Start() const		{ return m_Start; }
	TimeValue End() const		{ return m_End; }

	TimeValue Duration() const	{ return m_End-m_Start+TimeValue(1); } // end points included

	bool Contain( TimeValue t )	const { return (m_Start<=t && t<=m_End); }
	
	bool Contain( const Interval & interval ) const;
	bool StartIn( const Interval & interval ) const;
	bool EndIn( const Interval & interval )	const;
};

extern Interval FOREVER;

extern Timer g_Timer;

#endif
