/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#include <ACGL/Animations/Animation.hh>

using namespace ACGL;
using namespace ACGL::Animations;

AnimationList AnimationManager::mAnimations;

const bool& Animation::isInited() const
{
    return mInited;
}

const bool& Animation::isStopped() const
{
    return mStopped;
}

void Animation::restart()
{
    mInited = false;
}

void Animation::stop()
{
    mStopped = true;
}

void AnimationManager::push(const SharedAnimation& _animation)
{
    mAnimations.push_back(_animation);

    _animation->init();
}

void AnimationManager::update(uint_t _msec)
{
    AnimationList::iterator current = mAnimations.begin();

    while(current != mAnimations.end())
    {
        if((*current)->isStopped())
        {
            AnimationList::iterator toDelete = current;
            current++;
            mAnimations.erase(toDelete);
            continue;
        }

        (*current)->update(_msec);

        if((*current)->finished())
        {
            AnimationList::iterator toDelete = current;
            current++;
            mAnimations.erase(toDelete);
        }else current++;
    }
}

void AnimationManager::cleanUp()
{
    mAnimations.clear();
}

AnimationWait::AnimationWait(const uint_t _duration) :
    mTimeLeft(0),
    mDuration(_duration)
{
}

void AnimationWait::init()
{
    mTimeLeft = mDuration;

    mInited = true;
}

long AnimationWait::update(uint_t _msec)
{
    if(mTimeLeft >= _msec)
    {
        mTimeLeft -= _msec;

        return 0;
    }else
    {
        long timeRemaining = _msec - mTimeLeft;
        mTimeLeft = 0;

        return timeRemaining;
    }
}

bool AnimationWait::finished()
{
    return mTimeLeft == 0;
}

AnimationSequential::AnimationSequential(const int_t _loops) :
    mLoops(_loops),
    mCurrentPosition(),
    mAnimations()
{
}

AnimationSequential::~AnimationSequential()
{
    mAnimations.clear();
}

void AnimationSequential::init()
{
    mCurrentPosition = mAnimations.begin();
    mInited = true;

    for(AnimationList::iterator current = mAnimations.begin(); current != mAnimations.end(); current++)
        (*current)->restart();
}

long AnimationSequential::update(uint_t _msec)
{
    while(mCurrentPosition != mAnimations.end() && _msec > 0)
    {
        if((*mCurrentPosition)->isStopped())
        {
            AnimationList::iterator toDelete = mCurrentPosition;
            mCurrentPosition++;
            mAnimations.erase(toDelete);
            continue;
        }

        if(!(*mCurrentPosition)->isInited())
        {
            (*mCurrentPosition)->init();
        }

        _msec = (*mCurrentPosition)->update(_msec);

        if((*mCurrentPosition)->finished())
        {
            if(mLoops == Animation::EndlessNoRepeat)
            {
                AnimationList::iterator toDelete = mCurrentPosition;
                mCurrentPosition++;
                mAnimations.erase(toDelete);
            }else mCurrentPosition++;

            if(mCurrentPosition == mAnimations.end() && mLoops != 0)
            {
                if(mLoops > 0)
                    mLoops--;

                init();
            }
        }
    }

    return _msec;
}

bool AnimationSequential::finished()
{
    if(mLoops != 0) return false;
    else return mCurrentPosition == mAnimations.end();
}

void AnimationSequential::push_animation(const SharedAnimation& _animation)
{
    mAnimations.push_back(_animation);
}

AnimationParallel::AnimationParallel(const int_t _loops) :
    mLoops(_loops)
{

}

AnimationParallel::~AnimationParallel()
{
    mAnimations.clear();
}

void AnimationParallel::init()
{
    mRunningAnimations = mAnimations.size();

    for(AnimationList::iterator it = mAnimations.begin(); it != mAnimations.end(); it++)
    {
        (*it)->restart();
    }

    mInited = true;
}

long AnimationParallel::update(uint_t _msec)
{
    uint_t timeLeftMin = _msec;

    AnimationList::iterator current = mAnimations.begin();
    while(current != mAnimations.end())
    {
        if((*current)->isStopped())
        {
            if( !(*current)->finished() )
                mRunningAnimations--;

            AnimationList::iterator toDelete = current;
            current++;
            mAnimations.erase(toDelete);
            continue;
        }

        if(!(*current)->isInited())
        {
            (*current)->init();
        }

        if(!(*current)->finished())
        {
            uint_t timeLeft = (*current)->update(_msec);

            if(timeLeft < timeLeftMin)
                timeLeftMin = timeLeft;

            if((*current)->finished())
            {
                if(mLoops == Animation::EndlessNoRepeat)
                {
                    AnimationList::iterator toDelete = current;
                    current++;
                    mAnimations.erase(toDelete);
                    current--;
                }

                mRunningAnimations--;
            }
        }

        current++;
    }

    return timeLeftMin;
}

bool AnimationParallel::finished()
{
    if(mRunningAnimations == 0 && mLoops != 0)
    {
        if(mLoops > 0)
            mLoops--;

        init();

        return false;
    }

    return mRunningAnimations == 0;
}

void AnimationParallel::push_animation(const SharedAnimation& _animation)
{
    mRunningAnimations++;
    mAnimations.push_back(_animation);
}

#endif
