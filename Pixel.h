#ifndef IMAGE_SEGMENTATION_PIXEL_H
#define IMAGE_SEGMENTATION_PIXEL_H
#include <iostream>
#include <cmath>
#include <tuple>
#include <vector>
#include <algorithm>
namespace LibImageSegmentation
{
  ///One pixel in 2D space.
  template <class T> class Pixel
  {
    public:
    ///X coordinate.
    T x{};
    ///Y coordinate.
    T y{};
    virtual ~Pixel()=default;
    /** Constructor. Sets coordinates of the pixel to \[_x_,_y_\].
     */
    Pixel(T x,T y){this->set(x,y);}
    /** Constructor. Sets coordinates of the pixel to \[_p.x_,_p.y_\].
     */    
    template <class S> Pixel(const Pixel<S> &p){this->set(p.x,p.y);}
    /** Set coordinates of the pixel to \[_x_,_y_\].
     */    
    void set(T x,T y){this->x=x;this->y=y;}
    /** Set coordinates of the pixel to \[_p.x_,_p.y_\].
     */    
    template <class S> void set(const Pixel<S> &p){this->x=p.x;this->y=p.y;}
    /** Calculate squared Euclidean distance between this pixel and \[_x_,_y_\].
     */    
    template <class S, class U> double distance2(S x, U y) const {return((this->x-x)*(this->x-x)+(this->y-y)*(this->y-y));}
    /** Calculate Euclidean distance between this pixel and \[_x_,_y_\].
     */    
    template <class S, class U> double distance(S x, U y) const {return(sqrt(this->distance2(x,y)));}
    /** Calculate squared Euclidean distance between this pixel and _p2_.
     */    
    template <class S> double distance2(const Pixel<S> &p2) const {return(this->distance2(p2.x,p2.y));}
    /** Calculate Euclidean distance between this pixel and _p2_.
     */ 
    template <class S> double distance(const Pixel<S> &p2) const {return(sqrt(this->distance2(p2)));}
    /** Calculate distance of this pixel to a rectangle \[0,0\]-\[_width_,_height_\]. 
     */ 
    double distance_to_boundary(int width, int height)
    {
      double ret0=std::min<double>(std::abs(this->x),std::abs(this->y));
      double ret1=std::min<double>(std::abs(width-this->x-1),std::abs(height-this->y-1));
      return(std::min(ret0,ret1));
    }
    /** Calculate azimuth of a line connecting this pixel with \[_x_,_y_\].
     */ 
    template <class S> double angle_to(S x,S y) const {return(atan2(y-this->y,x-this->x));}
    /** Calculate azimuth of a line connecting this pixel with \[_x_,_y_\].
     */ 
    template <class S> double angle_to(const Pixel<S> &p2) const {return(this->angle_to(p2.x,p2.y));}
    /** Equality operator.
     * @return True, if this pixel and _p2_ have the same coordinates, false otherwise.
     */    
    bool operator==(const Pixel<T> &p2) const{return(this->x==p2.x && this->y==p2.y);}
    /**
     * @see bool Pixel::operator==()
     */
    bool operator!=(const Pixel<T> &p2) const{return(!((*this)==p2));}
    template <class S> friend Pixel<T> operator+(const Pixel<T> &lhs,const Pixel<S> &rhs){return(Pixel<T>(lhs.x+rhs.x,lhs.y+rhs.y));}
    template <class S> friend Pixel<T> operator+(const Pixel<T> &lhs,const S &rhs){return(Pixel<T>(lhs.x+rhs,lhs.y+rhs));}
    template <class S> friend Pixel<T> operator+(const S &lhs,const Pixel<T> &rhs){return(Pixel<T>(lhs+rhs.x,lhs+rhs.y));}
    template <class S> Pixel<T>& operator+=(const Pixel<S> &rhs) {this->x+=rhs.x;this->y+=rhs.y;return(*this);}
    template <class S> Pixel<T>& operator+=(const S &rhs) {this->x+=rhs;this->y+=rhs;return(*this);}
    template <class S> friend Pixel<T> operator-(const Pixel<T> &lhs,const Pixel<S> &rhs){return(Pixel<T>(lhs.x-rhs.x,lhs.y-rhs.y));}
    template <class S> friend Pixel<T> operator-(const Pixel<T> &lhs,const S &rhs){return(Pixel<T>(lhs.x-rhs,lhs.y-rhs));}
    template <class S> friend Pixel<T> operator-(const S &lhs,const Pixel<T> &rhs){return(Pixel<T>(lhs-rhs.x,lhs-rhs.y));}
    template <class S> Pixel<T>& operator-=(const Pixel<S> &rhs) {this->x-=rhs.x;this->y-=rhs.y;return(*this);}
    template <class S> Pixel<T>& operator-=(const S &rhs) {this->x-=rhs;this->y-=rhs;return(*this);}
    Pixel<T> operator-(){return(Pixel<T>(-this->x,-this->y));}
    template <class S> friend Pixel<T> operator*(const S &c,const Pixel<T> &rhs){return(Pixel<T>(c*rhs.x,c*rhs.y));}
    template <class S> friend Pixel<T> operator*(const Pixel<T> &lhs,const S &c){return(c*lhs);}
    /** Dot product of two pixels (2D vectors).
     */    
    template <class S> friend double operator*(const Pixel<T> &lhs,const Pixel<S> &rhs){return(lhs.x*rhs.x+lhs.y*rhs.y);}
    template <class S> Pixel<T>& operator*=(const S &rhs) {this->x*=rhs;this->y*=rhs;return(*this);}
    template <class S> friend Pixel<T> operator/(const Pixel<T> &lhs,const S &c){return(Pixel<T>(lhs.x/c,lhs.y/c));}
    template <class S> Pixel<T>& operator/=(S rhs) {this->x/=rhs;this->y/=rhs;return(*this);}
    friend std::ostream& operator<<(std::ostream& os, const Pixel& p){os << "[" << p.x << ", " << p.y << "]";return os;}
  };

  ///Typedef for centroids.
  using CenterOfGravity=Pixel<float>;

   
  ///One pixel in 2D space with associated value.
  template <class T> class PixelValue : public Pixel<int>
  {
    public:
    virtual ~PixelValue()=default;
    ///Value associated with the pixel.
    T value;
    /** Constructor. Sets coordinates of the pixel to \[_x_,_y_\] and its value to _value_.
     */
    PixelValue(int x,int y,const T &value) : Pixel(x,y) {this->value=value;}
    /** Set coordinates of the pixel to \[_x_,_y_\] and its value to _value_.
     */
    void set(int x,int y,const T &value){this->x=x;this->y=y;this->value=value;}
    /** Compare values of this pixel and _p2_ as follows: this->value>p2.value.
     */
    bool operator<(const PixelValue &p2) const {return(this->value>p2.value);}
    friend std::ostream& operator<<(std::ostream& os, const PixelValue& p){os << "[" << p.x << ", " << p.y << "](" << p.value << ")";return os;}
  };

  /** Calculate spatial quantile of pixels in a container.
   * This function is convenient e.g. for calculating spatial medians (_q_=0.5), which 
   * are (unlike conventional means) resistant to outliers. Unlike std::nth_element
   * this function does not change the order of the elements in the container.
   * @param begin Begin iterator of the container (e.g. std::vector::begin()).
   * @param end End iterator of the container (e.g. std::vector::end()).
   * @param q A number between 0 and 1.
   */
  template <class T,class Iterator> Pixel<T> get_pixels_quantile(const Iterator &begin,const Iterator &end,double q)
  {
    Pixel<T> ret(0,0);
    std::vector<Pixel<T> > pixelset;
    for(auto p=begin;p!=end;++p) pixelset.push_back(*p);
    auto quantileIndex=(double)pixelset.size()*q;
    if(quantileIndex>=pixelset.size()) quantileIndex=pixelset.size()-1;
    std::nth_element(pixelset.begin(),pixelset.begin()+quantileIndex,pixelset.end(),[](const Pixel<T> &a,const Pixel<T> &b)->bool{return(a.x<b.x);});
    ret.x=pixelset[quantileIndex].x;
    std::nth_element(pixelset.begin(),pixelset.begin()+quantileIndex,pixelset.end(),[](const Pixel<T> &a,const Pixel<T> &b)->bool{return(a.y<b.y);});
    ret.y=pixelset[quantileIndex].y;
    return(ret);

  }
  /** Calculate spatial quantile of pixels in a container.
   * This function is convenient e.g. for calculating spatial medians (_q_=0.5), which 
   * are (unlike conventional means) resistant to outliers. Unlike std::nth_element
   * this function does not change the order of the elements in the container.
   * @param c A container with  of arbitrary type (e.g. std::vector). The only requirement is that 
   * it provides begin and end iterators via member functions begin() and end().
   * @param q A number between 0 and 1.
   */  
  template <class T,class Container> Pixel<T> get_pixels_quantile(const Container &c,double q){return(get_pixels_quantile(c.begin(),c.end(),q));}
}

#endif

