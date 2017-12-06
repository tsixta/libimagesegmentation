#include "Line.h"
#include <math.h>

LibImageSegmentation::Line::Line(int initialX,int initialY,double angle,double length,const Pixel<int> &z1,const Pixel<int> &z2) : Line(initialX,initialY,angle,length)
{
  bool targetPositionDoesntFit=true;

  auto reset_length=[this,angle](double ratio,double &length)->bool
  {
    length*=ratio;
    this->targetPosition.set(this->calculate_target_point(angle,length));
    return(true);
  };
  while(targetPositionDoesntFit)
  {
    targetPositionDoesntFit=false;
    if(this->targetPosition.x<z1.x) targetPositionDoesntFit=reset_length((double)(this->initialPosition.x-z1.x)/(double)(this->initialPosition.x-this->targetPosition.x),length);
    if(this->targetPosition.x>z2.x) targetPositionDoesntFit=reset_length((double)(z2.x-this->initialPosition.x)/(double)(this->targetPosition.x-this->initialPosition.x),length);
    if(this->targetPosition.y<z1.y) targetPositionDoesntFit=reset_length((double)(this->initialPosition.y-z1.y)/(double)(this->initialPosition.y-this->targetPosition.y),length);
    if(this->targetPosition.y>z2.y) targetPositionDoesntFit=reset_length((double)(z2.y-this->initialPosition.y)/(double)(this->targetPosition.y-this->initialPosition.y),length);
  }
}
//-----------------------------------------------------------------------------
LibImageSegmentation::Pixel<int> LibImageSegmentation::Line::calculate_target_point(double angle,double length)const
{
  return(Pixel<int>(round(this->initialPosition.x+cos(angle)*length),
                    round(this->initialPosition.y+sin(angle)*length)));
}
//-----------------------------------------------------------------------------
LibImageSegmentation::Line::iterator::iterator(int n,const Line *object)
{
  this->initialPosition.set(object->initialPosition);
  this->targetPosition.set(object->targetPosition);
  if(n<0) 
  {
    this->out.set(this->targetPosition);
    this->hasEnded=true;
  }
  else
  {
    this->out.set(this->initialPosition);
    
    auto &x1=this->out.x;
    auto &y1=this->out.y;
    auto &x2=this->targetPosition.x;
    auto &y2=this->targetPosition.y;
    
    dx = x2 - x1;
    dy = y2 - y1;
    inx = dx > 0 ? 1 : -1;
    iny = dy > 0 ? 1 : -1;

    dx = abs(dx);
    dy = abs(dy);
     
    if(dx >= dy) 
    {
      dy <<= 1;
      e = dy - dx;
      dx <<= 1;
      this->firstBranch=true;
    } 
    else 
    {
      dx <<= 1;
      e = dx - dy;
      dy <<= 1;
      this->firstBranch=false;
    }

    for(int i=0;i<n;i++) ++(*this);
  }
}
//-----------------------------------------------------------------------------
bool LibImageSegmentation::Line::iterator::operator!=(const Line::iterator &other) const
{
  bool ret=true;
  if(other.hasEnded && this->hasEnded) ret=false;
  else if(!other.hasEnded && !this->hasEnded)
  {
    if(other.out!=this->out) ret=true;
    else ret=!(this->initialPosition==other.initialPosition && this->targetPosition==other.targetPosition);
  }
  return(ret);
}
//-----------------------------------------------------------------------------
LibImageSegmentation::Line::iterator& LibImageSegmentation::Line::iterator::operator++()
{
  auto &x1=this->out.x;
  auto &y1=this->out.y;
  auto &x2=this->targetPosition.x;
  auto &y2=this->targetPosition.y;
  if(this->firstBranch) 
  {
    if(x1 != x2) 
    {
      if(e >= 0) 
      {
        y1 += iny;
        e-= dx;
      }
      e += dy; x1 += inx;
    }
    else this->hasEnded=true;
  } 
  else 
  {
    if (y1 != y2) 
    {
      if(e >= 0) 
      {
        x1 += inx;
        e -= dy;
      }
      e += dx; y1 += iny;
    }
    else this->hasEnded=true;
  }
  return(*this);
}
