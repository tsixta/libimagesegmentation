#ifndef IMAGE_SEGMENTATION_LINE_H
#define IMAGE_SEGMENTATION_LINE_H
#include "Pixel.h"
#include <iterator>
namespace LibImageSegmentation
{
  ///Line in 2D space, its purpose is mainly to provide iterators.
  class Line
  {
    Pixel<int> initialPosition{0,0};
    Pixel<int> targetPosition{0,0};
    Pixel<int> calculate_target_point(double angle,double length)const;
    class iterator: public std::iterator<std::input_iterator_tag, int, int, const Pixel<int>*, Pixel<int> >
    {
      int dx, dy, inx, iny, e;
      bool firstBranch{};
      bool hasEnded=false;
      double length2;
      Pixel<int> initialPosition{0,0};
      Pixel<int> targetPosition{0,0};
      mutable Pixel<int> out{0,0};
      public:
      explicit iterator(int i,const Line *object);
      iterator& operator++();
      bool operator!=(const iterator &other) const;
      const reference &operator*() const{return(this->out);}
    };
    public:
    /** Constructor. Creates line starting at \[_initialX_,_initialY_\] with azimuth _angle_ radians and length _length_ pixels.
     */
    Line(int initialX,int initialY,double angle,double length) {this->initialPosition.set(initialX,initialY);this->targetPosition.set(this->calculate_target_point(angle,length));}
    /** Constructor. Creates line starting at \[_initialX_,_initialY_\] with azimuth _angle_ radians 
     *  and length up to _length_ pixels. The line is cropped such that it is completely inside 
     *  rectangle \[_z1.x_,_z1.y_\]-\[_z2.x_,_z2.y_\].
     */
    Line(int initialX,int initialY,double angle,double length,const Pixel<int> &z1,const Pixel<int> &z2);
    /** Constructor. Creates line starting at _initialZ_ with azimuth _angle_ radians and length _length_ pixels.
     */
    Line(const Pixel<int> &initialZ,double angle,double length) : Line(initialZ.x,initialZ.y,angle,length){}
    /** Constructor. Creates line starting at _initialZ_ with azimuth _angle_ radians 
     *  and length up to _length_ pixels. The line is cropped such that its endpoint is completely 
     *  inside rectangle \[_z1.x_,_z1.y_\]-\[_z2.x_,_z2.y_\].
     */
    Line(const Pixel<int> &initialZ,double angle,double length,const Pixel<int> &z1,const Pixel<int> &z2) : Line(initialZ.x,initialZ.y,angle,length,z1,z2){}
    /** Constructor. Creates line between _initialZ_ and _targetZ_.
     */
    Line(const Pixel<int> &initialZ,const Pixel<int> &targetZ){this->initialPosition.set(initialZ);this->targetPosition.set(targetZ);}
    ///Iterator to the beginning of the line.
    iterator begin() const {return(iterator(0,this));}
    ///Iterator to the end of the line.
    iterator end() const {return(iterator(-1,this));}
  };
}
#endif

