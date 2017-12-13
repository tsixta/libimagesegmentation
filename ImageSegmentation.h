#ifndef LIB_IMAGE_SEGMENTATION_MAIN_H
#define LIB_IMAGE_SEGMENTATION_MAIN_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <array>
#include <initializer_list>
#include <exception>
#include <string>
#include <type_traits>
#include "Pixel.h"
#include "Line.h"
namespace LibImageSegmentation
{
  ///Default underlying datatypes.
  class DefaultTypes
  {
    public:
    #ifdef DEFAULT_UNDERLYING_INT_TYPE
    ///Default integral type. It can be changed by defining DEFAULT_UNDERLYING_INT_TYPE macro during the compilation.
    using int_type=DEFAULT_UNDERLYING_INT_TYPE;
    ///Default unsigned integral type. It can be changed by defining DEFAULT_UNDERLYING_INT_TYPE macro during the compilation.
    using uint_type=unsigned DEFAULT_UNDERLYING_INT_TYPE;
    #else
    ///Default integral type. It can be changed by defining DEFAULT_UNDERLYING_INT_TYPE macro during the compilation.
    using int_type=int;
    ///Default unsigned integral type. It can be changed by defining DEFAULT_UNDERLYING_INT_TYPE macro during the compilation.
    using uint_type=unsigned int;
    #endif
    #ifdef DEFAULT_UNDERLYING_FLOAT_TYPE
    ///Default floating point type. It can be changed by defining DEFAULT_UNDERLYING_FLOAT_TYPE macro during the compilation.
    using float_type=DEFAULT_UNDERLYING_FLOAT_TYPE;
    #else
    ///Default floating point type. It can be changed by defining DEFAULT_UNDERLYING_FLOAT_TYPE macro during the compilation.
    using float_type=double;
    #endif
    ///ImageSegmentation<T,N> template parameter N that indicates multilabel segmentation.
    static const int SegmentationN=-1;
    ///ImageSegmentation<T,N> template parameter N that indicates binary segmentation.
    static const int SegmentationBlackWhiteN=-2;
  };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------  
  ///Exception handling.
  class Exception : public std::exception
  {
    protected:
    std::string message;
    public:
    Exception(){this->message="";}
    Exception(const std::string &message){this->message=message;}
    const char* what() const noexcept {return(this->message.c_str());}
  };
  ///Exception thrown by CImg
  struct CoreException : public Exception {CoreException(const std::string &message) : Exception(message){}};
  ///Bad format of a CSV file
  struct BadCSVFormatException : public Exception {BadCSVFormatException(const std::string &message) : Exception(message){}};
  ///Bad format of a segmentation image
  struct BadSegmentationFormatException : public Exception {BadSegmentationFormatException(int paletteSpectrum,int cimgSpectrum,const std::string &filename);};
  ///Bad combination of positions when stitching two images
  struct PositionException : public Exception {PositionException(const std::string &message) : Exception(message){}};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  /** Multichannel color.
   * @tparam T Underlying type.
   * @tparam N Number of channels.
   */
  template<class T,std::size_t N> struct Color : public std::array<T,N>
  {
    using std::array<T,N>::array;
    /** Constructor.
     *  Initializes all channels to default value (e.g. 0 for numerical underlying types _T_).
     */
    Color(){this->fill(T{});}
    /** Constructor.
     *  Initializes the channels from an initializer list. The caller is responsible to provide 
     *  an initializer list with correct length.
     */
    Color(const std::initializer_list<T> &c)
    {
      std::size_t i=0;
      for(auto &&it: c) {this->data()[i]=it;i++;}
    }
    /** Constructor.
     *  Initializes all channels to value _v_.
     */
    Color(const T &v) {this->fill(v);}
    /** Copy constructor.
     *  Initializes the channels to corresponding channels of _c_.
     */
    Color(const Color<T,N> &c){for(std::size_t i=0;i<N;i++) this->data()[i]=c[i];}
    /** Assignment operator.
     *  Sets the channels to corresponding channels of _c_.
     */
    template <class U> Color<T,N>& operator=(const Color<U,N> &c) {for(std::size_t i=0;i<N;i++) {this->data[i]=(T)c[i];}return(*this);}
    /** Assignment operator.
     *  Sets all channels to _v_.
     */
    Color<T,N>& operator=(const T &v) {for(std::size_t i=0;i<N;i++) {this->data[i]=v;} return(*this);}
    /** Inequality operator with std::vector.
     *  @return True if this color and _c_ differ in at least one channel, false otherwise.
     */
    bool operator!=(const std::vector<T> &c)const
    {
      if(c.size()==N)
      {
        for(std::size_t i=0;i<N;i++) 
          if(this->data()[i]!=c[i])
            return(true);
        return(false);
      }
      else return(true);
    }
    /** Inequality operator.
     *  @return True if this color and _c_ differ in at least one channel, false otherwise.
     */
    bool operator!=(const Color<T,N> &c)const
    {
      for(std::size_t i=0;i<N;i++) if(this->data()[i]!=c[i])return(true);
      return(false);
    }
    /** Equality operator.
     *  @see operator!=(const Color<T,N> &c)const
     */
    bool operator==(const Color<T,N> &c)const{return(!(*this!=c));}
    /** Equality operator.
     *  @see operator!=(const std::vector<T> &c)const
     */
    bool operator==(const std::vector<T> &c)const{return(!(*this!=c));}

    //Auto-generated arithmetical operators
    friend Color<T,N> operator+(const Color<T,N> &c1,const Color<T,N> &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]+=c2[i];}return(ret);}
    friend Color<T,N> operator+(const Color<T,N> &c1,const T &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]+=c2;}return(ret);}
    friend Color<T,N> operator+(const T &c1,const Color<T,N> &c2) {Color<T,N> ret(c2);for(std::size_t i=0;i<N;i++) {ret[i]=c1+ret[i];}return(ret);}
    Color<T,N>& operator+=(const Color<T,N> &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]+=c2[i];} return(*this);}
    Color<T,N>& operator+=(const T &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]+=c2;} return(*this);}    
    friend Color<T,N> operator-(const Color<T,N> &c1,const Color<T,N> &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]-=c2[i];}return(ret);}
    friend Color<T,N> operator-(const Color<T,N> &c1,const T &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]-=c2;}return(ret);}
    friend Color<T,N> operator-(const T &c1,const Color<T,N> &c2) {Color<T,N> ret(c2);for(std::size_t i=0;i<N;i++) {ret[i]=c1-ret[i];}return(ret);}
    Color<T,N>& operator-=(const Color<T,N> &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]-=c2[i];} return(*this);}
    Color<T,N>& operator-=(const T &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]-=c2;} return(*this);}            
    ///Element-wise product.
    friend Color<T,N> operator*(const Color<T,N> &c1,const Color<T,N> &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]*=c2[i];}return(ret);}
    friend Color<T,N> operator*(const Color<T,N> &c1,const T &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]*=c2;}return(ret);}
    friend Color<T,N> operator*(const T &c1,const Color<T,N> &c2) {Color<T,N> ret(c2);for(std::size_t i=0;i<N;i++) {ret[i]=c1*ret[i];}return(ret);}
    ///Element-wise product.
    Color<T,N>& operator*=(const Color<T,N> &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]*=c2[i];} return(*this);}
    Color<T,N>& operator*=(const T &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]*=c2;} return(*this);}
    ///Element-wise division.
    friend Color<T,N> operator/(const Color<T,N> &c1,const Color<T,N> &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]/=c2[i];}return(ret);}
    friend Color<T,N> operator/(const Color<T,N> &c1,const T &c2){Color<T,N> ret(c1);for(std::size_t i=0;i<N;i++) {ret[i]/=c2;}return(ret);}
    friend Color<T,N> operator/(const T &c1,const Color<T,N> &c2) {Color<T,N> ret(c2);for(std::size_t i=0;i<N;i++) {ret[i]=c1/ret[i];}return(ret);}
    ///Element-wise division.
    Color<T,N>& operator/=(const Color<T,N> &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]/=c2[i];} return(*this);}
    Color<T,N>& operator/=(const T &c2) {for(std::size_t i=0;i<N;i++) {this->data()[i]/=c2;} return(*this);}

    Color<T,N> operator-() {Color<T,N> ret(*this);for(std::size_t i=0;i<N;i++) {ret[i]=-ret[i];} return(ret);}    
    
    friend std::ostream &operator<<(std::ostream &out, const Color<T,N> &c)
    {
      const auto d=c.data();
      out << "[";
      for(std::size_t i=0;i<N;i++) 
      {
        out << d[i]; 
        if(i<N-1) out << ",";
      } 
      out << "]";
      return(out);
    }
  };
  ///Type alias for RGB color.
  using RGBColor=Color<DefaultTypes::int_type,3>;
  ///Type alias for RGB color.
  using ColorRGB=RGBColor;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  /** Container of colors.
   * @tparam T Underlying type.
   * @tparam N Number of channels.
   */  
  template <class T,std::size_t N> struct Palette : public std::vector<Color<T,N> > 
  {
    static const int colorNotRecognized=-1;
    template <class U> int get_color_index(const U &c)const
    {
      int ret=colorNotRecognized;
      const auto data=this->data();
      for(size_t i=0;i<this->size();i++)
      {
        if(data[i]==c) {ret=i;break;}
      }
      return(ret);
    }
    Color<T,N> get_color_for_segmentation(int index)const
    {
      if(index>0) index=((index-1) % (this->size()-1))+1;
      else index=0;
      return(this->data()[index]);
    }
    ///Number of channels of colors in the container.
    const int spectrum=N;
  };
  ///Standard segmentation palette.
  struct StandardPalette : public Palette<int,3> {StandardPalette();};
  ///Bright segmentation palette for better visibility.
  struct BrightPalette : public Palette<int,3> {BrightPalette();};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  template <class T> class __PrivateCImgWrapper
  {
    void *cimg;
    int width=-1;
    int height=-1;
    int spectrum=-1;
    __PrivateCImgWrapper(const std::string &filename);
    __PrivateCImgWrapper(int width,int height,int spectrum);
    __PrivateCImgWrapper(int spectrum,const std::string &text,T *foregroundColor,T *backgroundColor,int size);
    void init_dimensions(int width,int height,int spectrum){this->width=width;this->height=height;this->spectrum=spectrum;}
    T _get(int x,int y,int c)const;
    void get(int x,int y,T &res)const{res=this->_get(x,y,0);}
    void get(int x,int y,std::vector<T> &res)const{res.resize(this->spectrum);for(int c=0;c<this->spectrum;c++) res[c]=this->_get(x,y,c);}
    template <std::size_t N> void get(int x,int y,Color<T,N> &res)const{for(size_t c=0;c<N;c++) res[c]=this->_get(x,y,c);}
    double get_average(int x,int y)const;
    void set(int x,int y,int c,T value);
    void set(int x,int y,T value){this->set(x,y,0,value);}
    template <std::size_t N> void set(int x,int y,const Color<T,N> &color){for(size_t c=0;c<N;c++) this->set(x,y,c,color[c]);}
    void resize(double m,int cimgInterpolationType=1);
    void save(const std::string &filename)const;
    ~__PrivateCImgWrapper();
    template<class TT,int N> friend class ImageSegmentation;
  };
  
  //Enums
  ///Verbosity of image loading.
  enum class Verbose{Silent=0,Normal=1};
  #ifdef None //Conflict with CImg macro None
    #undef None
    #define __REDEFINE_CIMG_NONE
  #endif
  ///Interpolation types.
  enum class CImgInterpolation{RawMemory=-1,None=0,NearestNeighbor=1,MovingAverage=2,Linear=3,Grid=4,Cubic=5,Lanczos=6};
  #ifdef __REDEFINE_CIMG_NONE
    #define None 0
  #endif
  ///Positions for stitching.
  enum class Position{Left,Right,Top,Bottom,Center};  
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  /** Segmentation, single-channel or multichannel image.
   * @tparam T Underlying type.
   * @tparam N Number of channels.
   */ 
  template <class T,int N> class ImageSegmentation
  {
    protected:
    enum class Message{Note,Warning,Error};
    static std::string compose_message(Message type,const std::string &method,const std::string &message)
    {
      std::string ret;
      if(type==Message::Note) ret="Note: ";
      else if(type==Message::Warning) ret="Warning: ";
      else if(type==Message::Error) ret="Error: ";
      return(ret+"["+method+"]: "+message);
    }
    public:
    /** Data type representing color of one pixel. If template parameter N<=1 (single channel image or segmentation)
     *  color_type is directly the underlying image type T. For multichannel images the color_type is Color<T,N>
     */
    using color_type=typename std::conditional<N<=1, T, Color<T,N> >::type;
    protected:
    using cimg_underlying_type=typename std::conditional<std::is_same<T,uint8_t>::value,uint8_t, 
                                typename std::conditional<std::is_same<T,uint16_t>::value,uint16_t,
                                typename std::conditional<std::is_same<T,uint32_t>::value,uint32_t,
                                typename std::conditional<std::is_same<T,uint64_t>::value,uint64_t,    
                                typename std::conditional<std::is_floating_point<T>::value,DefaultTypes::float_type,
                                typename std::conditional<std::is_unsigned<T>::value,DefaultTypes::uint_type,DefaultTypes::int_type
                                >::type>::type>::type>::type>::type>::type;
    using cimg_color_type=typename std::conditional<N<=1, cimg_underlying_type, Color<cimg_underlying_type,N> >::type;
    public:

    //Image data
    ///Verbosity of image loading.
    Verbose verbose=Verbose::Normal;
    ///Width (number of columns).
    int width=0;
    ///Height (number of rows).
    int height=0;
    ///Dimensions \[width,height\].
    Pixel<int> size(){return(Pixel<int>(this->width,this->height));}
    ///Number of channels.
    const int spectrum=N<=1?1:N;
    ///Pixel data.
    color_type** data=nullptr;
    /** Constructor.
     *  Creates an empty image/segmentation.
     */
    ImageSegmentation(Verbose verbose=Verbose::Normal){this->verbose=verbose;}
    /** Multilabel segmentation constructor.
     *  Loads a multilabel segmentation from file.
     */
    template <int NN=N> ImageSegmentation(const typename std::enable_if<NN==DefaultTypes::SegmentationN,std::string>::type &filename,
                                          Verbose verbose=Verbose::Normal) : ImageSegmentation(verbose) {this->load<NN>(filename);}
    /** Multilabel segmentation constructor.
     *  Loads a multilabel segmentation from file using custom palette.
     */
    template <class PT,std::size_t PN,int NN=N> ImageSegmentation(const typename std::enable_if<NN==DefaultTypes::SegmentationN,std::string>::type &filename,
                                          const Palette<PT,PN> &palette,
                                          Verbose verbose=Verbose::Normal) : ImageSegmentation(verbose) {this->load<PT,PN,NN>(filename,palette);}
    /** Binary segmentation constructor.
     *  Loads a binary (black & white) segmentation from file.
     */
    template <int NN=N> ImageSegmentation(const typename std::enable_if<NN==DefaultTypes::DefaultTypes::SegmentationBlackWhiteN,std::string>::type &filename,
                                          int bitdepth=-1,
                                          Verbose verbose=Verbose::Normal) : ImageSegmentation(verbose) {this->load<NN>(filename,bitdepth);}
    /** Multichannel image constructor.
     *  Loads a multichannel image from file.
     */
    template <int NN=N> ImageSegmentation(const typename std::enable_if<1<NN,std::string>::type &filename,
                                          Verbose verbose=Verbose::Normal) : ImageSegmentation(verbose) {this->load<NN>(filename);}
    /** Single channel image constructor.
     *  Loads a single channel image from file.
     *  @param filename Path to the loaded file.
     *  @param averageIfRGB If false and the input image has three channels (RGB), gray value of each pixel
     *                      is calculated as 0.2989*R+0.5871*G+0.1140*B instead of simple average.
     *  @param bitdepth Expected bitdepth of the file. Valid values are -1 (default), 8, 16, 32 and 64.
     *                  Value -1 means, that the bitdepth is estimated automatically. This argument is only useful
     *                  for images with floating point underlying type, which are normalized to \[0,1\] interval.
     *  @param verbose Verbosity.
     */
    template <int NN=N> ImageSegmentation(const typename std::enable_if<!(NN==DefaultTypes::SegmentationN || NN==DefaultTypes::DefaultTypes::SegmentationBlackWhiteN || 1<NN),std::string>::type &filename,
                                          bool averageIfRGB=false,
                                          int bitdepth=-1,
                                          Verbose verbose=Verbose::Normal) : ImageSegmentation(verbose) {this->load<NN>(filename,averageIfRGB,bitdepth);}
    /** Constructor.
     *  Creates a segmentation or image of size \[width,height\].
     */    
    ImageSegmentation(int width,int height,Verbose verbose=Verbose::Normal) : ImageSegmentation(verbose){this->reallocate(width,height);}
    /** Copy Constructor.
     *  Allocates a segmentation or image of size \[img2.width,img2.height\] and copies data from img2.
     */    
    ImageSegmentation(const ImageSegmentation<T,N> &img2){this->verbose=img2.verbose;this->copy_data(img2);}
    /** Assignment operator.
     *  Reallocates a segmentation or image of size \[img2.width,img2.height\] and copies data from img2.
     */    
    ImageSegmentation<T,N>& operator=(const ImageSegmentation<T,N> &img2){this->release();this->verbose=img2.verbose;this->copy_data(img2);return(*this);}
    ~ImageSegmentation() {this->release();}
    
    //-------------------------------------------------------------------------
    /** Reallocate the image to size \[size.x,size.y\].
     *  The old data are destroyed and the content is lost.
     */    
    void reallocate(const Pixel<int> &size){this->reallocate(size.x,size.y);}
    /** Reallocate the image to size \[width,height\].
     *  The old data are destroyed and the content is lost.
     */
    void reallocate(int width,int height)
    {
      int x,y;
      if(width!=this->width || height!=this->height)
      {
        this->release();
        this->width=width;
        this->height=height;
        if(this->width>0 && this->height>0)
        {
          this->data=new color_type*[this->width];
          for(x=0;x<this->width;x++)
          {
            this->data[x]=new color_type[this->height];
            for(y=0;y<this->height;y++) this->data[x][y]=color_type{}; 
          }
        }
      }
    }
    //-------------------------------------------------------------------------
    /** Deallocate the data and set the dimensions to '[0,0\].
     */
    void release()
    {
      if(this->data!=nullptr)
      {
        for(int x=0;x<this->width;x++) delete []this->data[x];
        delete []this->data;
      }
      this->width=0;
      this->height=0;
      this->data=nullptr;
    }  
    //-------------------------------------------------------------------------
    /** Init data in rectangle \[_z1.x_,_z1.y_\]-\[_z2.x_,_z2.y_\] to value _color_.
     */
    template <class U, class V> void init_data(const color_type &color,const Pixel<U> &z1,const Pixel<V> &z2)
    {
      this->init_data(color,z1.x,z1.y,z2.x,z2.y);
    }
    /** Init data to value _color_.
     *  Optionally the caller may specify a bounding box \[_x1_,_y1_\]-\[_x2_,_y2_\] to be initialized.
     *  If any of these arguments are smaller than 0 the corresponding coordinate of the bounding box
     *  is set according to the image size: _x1_, _y1_ -> 0, _x2_ -> width-1 and _y2_ -> height-1
     */
    void init_data(const color_type &color,int x1=-1,int y1=-1,int x2=-1,int y2=-1)
    {
      int x,y;
      if(x1<0) x1=0;
      if(y1<0) y1=0;
      if(x2<0) x2=this->width-1;
      if(y2<0) y2=this->height-1;
      for(x=x1;x<=x2;x++)
        for(y=y1;y<=y2;y++)
          this->data[x][y]=color;
    }
    //-------------------------------------------------------------------------
    /** Copy data of _img2_ to this image/segmentation.
     *  The data are shifted by vector \[_targetZ.x_,_targetZ.y_\] (i.e. pixel \[0,0\] of _img2_
     *  will be copied to \[_targetZ.x_,_targetZ.y_\] in this image/segmentation). If this image
     *  is not big enough for _img2_ data, it is reallocated and it's original content is lost.
     */    
    void copy_data(const ImageSegmentation<T,N> &img2,const Pixel<int> &targetZ){this->copy_data(img2,targetZ.x,targetZ.y);}
    /** Copy data of _img2_ to this image/segmentation.
     *  The data are shifted by vector \[_targetX_,_targetY_\] (i.e. pixel \[0,0\] of _img2_
     *  will be copied to \[_targetX_,_targetY_\] in this image/segmentation). If this image
     *  is not big enough for _img2_ data, it is reallocated and it's original content is lost.
     */    
    void copy_data(const ImageSegmentation<T,N> &img2,int targetX=0,int targetY=0)
    {
      if(this->width<img2.width+targetX || this->height<img2.height+targetY) this->reallocate(img2.width,img2.height);
      int x,y;
      int leftX=std::max(0,-targetX);
      int topY=std::max(0,-targetY);
      for(x=leftX;x<img2.width;x++)
        for(y=topY;y<img2.height;y++) 
          this->data[x+targetX][y+targetY]=img2.data[x][y];
    }
    //-------------------------------------------------------------------------
    /** Determines, whether pixel _z_ is inside this image/segmentation.
     */
    bool is_pixel_inside(const Pixel<int> &z)const{return(this->is_pixel_inside(z.x,z.y));}
    /** Determines, whether pixel \[_x_,_y_\] is inside this image/segmentation.
     */
    bool is_pixel_inside(int x,int y)const{return(x>=0 && x<this->width && y>=0 && y<this->height);}
    //-------------------------------------------------------------------------
    /** Determines, whether pixel _z_ is inside a bounding box 
     *  \[_boundingBoxZ1.x_,_boundingBoxZ1.y_\]-\[_boundingBoxZ2.x_,_boundingBoxZ2.y_\].
     */    
    bool is_pixel_inside(const Pixel<int> &z,const Pixel<int> &boundingBoxZ1,const Pixel<int> &boundingBoxZ2)const
    {
      return(this->is_pixel_inside(z.x,z.y,boundingBoxZ1,boundingBoxZ2));
    }
    /** Determines, whether pixel \[_x_,_y_\] is inside a bounding box 
     *  \[_boundingBoxZ1.x_,_boundingBoxZ1.y_\]-\[_boundingBoxZ2.x_,_boundingBoxZ2.y_\].
     */    
    bool is_pixel_inside(int x,int y,const Pixel<int> &boundingBoxZ1,const Pixel<int> &boundingBoxZ2)const
    {
      return(x>=boundingBoxZ1.x && x<=boundingBoxZ2.x && y>=boundingBoxZ1.y && y<=boundingBoxZ2.y);
    }
    //-------------------------------------------------------------------------
    /** Set value of pixel _z_ to _value_.
     *  If \[_x_,_y_\] is not inside the segmentation/image no pixel is changed.
     *  @return True if _z_ is inside the segmentation/image, false otherwise.
     */
    template <class U> bool set_pixel_safe(const Pixel<U> &z,const color_type &value) {return(set_pixel_safe(z.x,z.y,value));}
    /** Set value of pixel \[_x_,_y_\] to _value_.
     *  If \[_x_,_y_\] is not inside the segmentation/image no pixel is changed.
     *  @return True if \[_x_,_y_\] is inside the segmentation/image, false otherwise.
     */
    bool set_pixel_safe(int x,int y, const color_type &value)
    {
      bool ret=false;
      if(this->is_pixel_inside(x,y))
      {
        this->data[x][y]=value;
        ret=true;
      }
      return(ret);
    }
    
    //-------------------------------------------------------------------------
    /** Return value of pixel _z_.
     *  If _z_ is not inside the segmentation/image this method returns a color 
     *  initialized to -1 if the underlying type of the image is signed and a default initialized 
     *  color otherwise.
     */
    template <class U> color_type get_pixel_safe(const Pixel<U> &z)const{return(this->get_pixel_safe(z.x,z.y));}
    /** Return value of pixel \[_x_,_y_\].
     *  If \[_x_,_y_\] is not inside the segmentation/image this method returns a color 
     *  initialized to -1 if the underlying type of the image is signed and a default initialized 
     *  color otherwise.
     */
    color_type get_pixel_safe(int x,int y)const
    {
      color_type ret{};
      if(std::is_signed<T>::value) ret=color_type{-1};
      if(this->is_pixel_inside(x,y)) ret=this->data[x][y];
      return(ret);
    }
    //-------------------------------------------------------------------------
    /** Return number of occurences of _l_ in the data.
     */
    int count_label(const color_type &l)
    {
      int ret=0;
      int x,y;
      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          ret+=this->data[x][y]==l;
      return(ret);
    }
    //-------------------------------------------------------------------------
    /** Resize segmentation/image.
     *  This method is defined only if the underlying type of the segmentation/image is arithmetic.
     */
    template <class TT=T> void resize(typename std::enable_if<std::is_arithmetic<TT>::value,double>::type scale,CImgInterpolation interpolationType=CImgInterpolation::Linear)
    {
      static_assert(std::is_same<TT,T>::value,"Error [ImageSegmentation::resize]: Cannot change the underlying type.");
      int x,y;
      __PrivateCImgWrapper<cimg_underlying_type> cimg(this->width,this->height,this->spectrum);

      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          cimg.set(x,y,this->data[x][y]);
      cimg.resize(scale,(int)interpolationType);
      this->reallocate(cimg.width,cimg.height);

      cimg_color_type cc{};
      for(x=0;x<this->width;x++)
      {
        for(y=0;y<this->height;y++)
        {
          cimg.get(x,y,cc);
          this->data[x][y]=cc;
        }
      }
    }
    //-------------------------------------------------------------------------
    /** Cut a rectangle from this image.
     *  @param z1 Left-top corner of the cut rectangle.
     *  @param z2 Right-bottom corner of the cut rectangle.
     *  @param res The resulting cut (data). This method automatically reallocates _res_ such that 
     *         its size is the same of the size of the cut. If \[_x1_,_y1_\]-\[_x2_,_y2_\] does not
     *         lie completely inside the segmentation/image, the outside pixels are omited and the size
     *         of the cut is changed accordingly.
     */     
    void cut(const Pixel<int> &z1, const Pixel<int> &z2, ImageSegmentation<T,N> &res)const {this->cut(z1.x,z1.y,z2.x,z2.y,res);}
    //-------------------------------------------------------------------------
    /** Cut a rectangle from this image.
     *  @param x1 X coordinate of the left-top corner of the cut rectangle.
     *  @param y1 Y coordinate of the left-top corner of the cut rectangle.
     *  @param x2 X coordinate of the right-bottom corner of the cut rectangle.
     *  @param y2 Y coordinate of the right-bottom corner of the cut rectangle.
     *  @param res The resulting cut (data). This method automatically reallocates _res_ such that 
     *         its size is the same of the size of the cut. If \[_x1_,_y1_\]-\[_x2_,_y2_\] does not
     *         lie completely inside the segmentation/image, the outside pixels are omited and the size
     *         of the cut is changed accordingly.
     */
    void cut(int x1,int y1,int x2,int y2,ImageSegmentation<T,N> &res)const
    {
      int resRealX1=0,resRealY1=0,resRealX2=0,resRealY2=0;
      this->cut(x1,y1,x2,y2,res,resRealX1,resRealY1,resRealX2,resRealY2);
    }
    //-------------------------------------------------------------------------
    /** Cut a rectangle from this image.
     *  @param z1 Left-top corner of the cut rectangle.
     *  @param z2 Right-bottom corner of the cut rectangle.
     *  @param res The resulting cut (data). This method automatically reallocates _res_ such that 
     *         its size is the same of the size of the cut. If \[_x1_,_y1_\]-\[_x2_,_y2_\] does not
     *         lie completely inside the segmentation/image, the outside pixels are omited and the size
     *         of the cut is changed accordingly.
     *  @param resRealX1 X coordinate of the resulting cut rectangle that corresponds to x=0 in _res_.
     *  @param resRealY1 Y coordinate of the resulting cut rectangle that corresponds to y=0 in _res_.
     *  @param resRealX2 X coordinate of the resulting cut rectangle that corresponds to x=res.width-1.
     *  @param resRealY2 Y coordinate of the resulting cut rectangle that corresponds to y=res.height-1.
     */    
    void cut(const Pixel<int> &z1,const Pixel<int> &z2,ImageSegmentation<T,N> &res,Pixel<int> &resRealZ1,Pixel<int> &resRealZ2)const
    {
      this->cut(z1.x,z1.y,z2.x,z2.y,res,resRealZ1.x,resRealZ1.y,resRealZ2.x,resRealZ2.y);
    }
    //-------------------------------------------------------------------------
    /** Cut a rectangle from this image.
     *  @param x1 X coordinate of the left-top corner of the cut rectangle.
     *  @param y1 Y coordinate of the left-top corner of the cut rectangle.
     *  @param x2 X coordinate of the right-bottom corner of the cut rectangle.
     *  @param y2 Y coordinate of the right-bottom corner of the cut rectangle.
     *  @param res The resulting cut (data). This method automatically reallocates _res_ such that 
     *         its size is the same of the size of the cut. If \[_x1_,_y1_\]-\[_x2_,_y2_\] does not
     *         lie completely inside the segmentation/image, the outside pixels are omited and the size
     *         of the cut is changed accordingly.
     *  @param resRealX1 X coordinate of the resulting cut rectangle that corresponds to x=0 in _res_.
     *  @param resRealY1 Y coordinate of the resulting cut rectangle that corresponds to y=0 in _res_.
     *  @param resRealX2 X coordinate of the resulting cut rectangle that corresponds to x=res.width-1.
     *  @param resRealY2 Y coordinate of the resulting cut rectangle that corresponds to y=res.height-1.
     */
    void cut(int x1,int y1,int x2,int y2,ImageSegmentation<T,N> &res,int &resRealX1,int &resRealY1,int &resRealX2,int &resRealY2)const
    {
      int realX1=std::max(x1,0);
      int realY1=std::max(y1,0);
      int realX2=std::min(x2,this->width-1);
      int realY2=std::min(y2,this->height-1);
      int x,y;
      if(realX1<=realX2 && realY1<=realY2)
      {
        res.reallocate(realX2-realX1+1,realY2-realY1+1);
        for(x=realX1;x<=realX2;x++)
          for(y=realY1;y<=realY2;y++)
            res.data[x-realX1][y-realY1]=this->data[x][y];
      }
      resRealX1=realX1;
      resRealY1=realY1;
      resRealX2=realX2;
      resRealY2=realY2;
    }
    //-------------------------------------------------------------------------
    
    /** Stitch another segmentation/image to this segmentation/image.
     *  @param img2 Image to be stitched.
     *  @param primaryAlignment Position of the added image relative to this image. Possible values are 
     *         Position::Left, Position::Right, Position::Top and Position::Bottom.
     *  @param secondaryAlignment Secondary alignment of the added image. If the primary alignment is left/right
     *         the secondary alignment might be Position::Top, Position::Center or Position::Bottom.
     *         If the primary alignment is top/bottom the secondary alignment might be Position::Left, 
     *         Position::Center or Position::Right.
     *  @param backgroundColor Color of pixels that are covered neither by this segmentation/image nor _img2_.
     *  @param dividingLineThickness Thickness of the dividing line in pixels.
     *  @param dividingLineColor Color of the dividing line.
     */
    void stitch(const ImageSegmentation<T,N> &img2,Position primaryAlignment,Position secondaryAlignment,const color_type &backgroundColor=color_type{},int dividingLineThickness=0,const color_type &dividingLineColor=color_type{})
    {
      int originalX=0,originalY=0;
      int appendedX=0,appendedY=0;
      int newWidth=0,newHeight=0;
      int dx1=0,dx2=0,dy1=0,dy2=0;
      if(dividingLineThickness<0) dividingLineThickness=0;
      //Determine new dimensions
      if(primaryAlignment==Position::Left)
      {
        newWidth=this->width+dividingLineThickness+img2.width;
        newHeight=std::max(this->height,img2.height);
        dx1=img2.width;dx2=dx1+dividingLineThickness-1;dy2=newHeight-1;
        originalX=img2.width+dividingLineThickness;
        if(secondaryAlignment==Position::Bottom) {appendedY=newHeight-img2.height;originalY=newHeight-this->height;}
        else if(secondaryAlignment==Position::Center) {appendedY=newHeight/2-img2.height/2;originalY=newHeight/2-this->height/2;}
      }
      else if(primaryAlignment==Position::Right)
      {
        newWidth=this->width+dividingLineThickness+img2.width;
        newHeight=std::max(this->height,img2.height);
        dx1=this->width;dx2=dx1+dividingLineThickness-1;dy2=newHeight-1;
        appendedX=this->width+dividingLineThickness;
        if(secondaryAlignment==Position::Bottom) {appendedY=newHeight-img2.height;originalY=newHeight-this->height;}
        else if(secondaryAlignment==Position::Center) {appendedY=newHeight/2-img2.height/2;originalY=newHeight/2-this->height/2;}
      }
      else if(primaryAlignment==Position::Top)
      {
        newWidth=std::max(this->width,img2.width);
        newHeight=this->height+dividingLineThickness+img2.height;
        dx2=newWidth-1;dy1=img2.height;dy2=dy1+dividingLineThickness-1;
        originalY=img2.height+dividingLineThickness;
        if(secondaryAlignment==Position::Right) {appendedX=newWidth-img2.width;originalX=newWidth-this->width;}
        else if(secondaryAlignment==Position::Center) {appendedX=newWidth/2-img2.width/2;originalX=newWidth/2-this->width/2;}
      }
      else if(primaryAlignment==Position::Bottom)
      {
        newWidth=std::max(this->width,img2.width);
        newHeight=this->height+dividingLineThickness+img2.height;
        dx2=newWidth-1;dy1=this->height;dy2=dy1+dividingLineThickness-1;
        appendedY=this->height+dividingLineThickness;
        if(secondaryAlignment==Position::Right) {appendedX=newWidth-img2.width;originalX=newWidth-this->width;}
        else if(secondaryAlignment==Position::Center) {appendedX=newWidth/2-img2.width/2;originalX=newWidth/2-this->width/2;}
      }
      else
      {
        throw(PositionException(compose_message(Message::Error,"ImageSegmentation::stitch","Cannot append image at position "+std::to_string((int)primaryAlignment)+".")));
      }
      //Reallocate and draw
      ImageSegmentation<T,N> backup(*this);
      ImageSegmentation<T,N> backup2(img2);//In case *this==img2
      this->reallocate(newWidth,newHeight);
      this->init_data(backgroundColor);
      this->init_data(dividingLineColor,dx1,dy1,dx2,dy2);
      this->copy_data(backup,originalX,originalY);
      this->copy_data(backup2,appendedX,appendedY);
    }
    //-------------------------------------------------------------------------
    /** Draw line.
     *  The line starts at _initialZ_ with azimuth _angle_ and is at most _length_ pixels long.
     */
    void draw_line(const Pixel<int> &initialZ,double angle,double length,const color_type &color){this->draw_line(initialZ.x,initialZ.y,angle,length,color);}
    /** Draw line.
     *  The line starts at \[_initialX_,_initalY_\] with azimuth _angle_ and is at most _length_ pixels long.
     */
    void draw_line(int initialX,int initialY,double angle,double length,const color_type &color)
    {
      Line l(initialX,initialY,angle,length,Pixel<int>(0,0),Pixel<int>(this->width-1,this->height-1));
      this->draw(l.begin(),l.end(),color);
    }
    //-------------------------------------------------------------------------
    /** Draw line.
     *  The line starts at _initialZ_ and ends at _targetZ_.
     */
    void draw_line(const Pixel<int> &initialZ, const Pixel<int> &targetZ,const color_type &color)
    {
      Line l(initialZ,targetZ);
      this->draw(l.begin(),l.end(),color);
    }
    //-------------------------------------------------------------------------
    /** Draw pixels.
     *  This method iterates over a pixel container represented by its _begin_ and _end_ iterators
     *  and draws each pixel using color _color_. 
     *  @see void draw_unsafe(const Iterator &begin,const Iterator &end,const color_type &color)
     */
    template <class Iterator> void draw(const Iterator &begin,const Iterator &end,const color_type &color)
    {
      for(auto p=begin;p!=end;++p) this->set_pixel_safe(*p,color);
    }
    //-------------------------------------------------------------------------    
    /** Draw pixels.
     *  This method iterates over a pixel container represented by its _begin_ and _end_ iterators
     *  and draws each pixel using color _color_. 
     *  This method is faster than ImageSegmentation::draw because it does not check whether the pixels are 
     *  inside the segmentation/image. It is therefore responsibility of the caller to make sure, that all drawn pixels
     *  meet this condition because changing a value of an outside pixel may (and probably will)
     *  cause segmentation fault.
     *  @see void draw(const Iterator &begin,const Iterator &end,const color_type &color)
     */
    template <class Iterator> void draw_unsafe(const Iterator &begin,const Iterator &end,const color_type &color)
    {
      for(auto p=begin;p!=end;++p) this->data[(*p).x][(*p).y]=color;
    }
    //-------------------------------------------------------------------------
    /** Draw circle.
     */
    void draw_circle(const Pixel <int> &center,int radius,const color_type &color){this->draw_circle(center.x,center.y,radius,color);}
    /** Draw circle.
     */
    void draw_circle(int centerX, int centerY, int radius,const color_type &color)
    {
      int f=1-radius;
      int ddFX=1;
      int ddFY=-2*radius;
      int x=0;
      int y=radius;
      
      this->set_pixel_safe(centerX,centerY+radius,color);
      this->set_pixel_safe(centerX,centerY-radius,color);
      this->set_pixel_safe(centerX+radius,centerY,color);
      this->set_pixel_safe(centerX-radius,centerY,color); 
      while(x<y)
      {
        if(f>=0) 
        {
          y--;
          ddFY+=2;
          f+=ddFY;
        }
        x++;
        ddFX+=2;
        f+=ddFX;
        this->set_pixel_safe(centerX+x,centerY+y,color);
        this->set_pixel_safe(centerX-x,centerY+y,color);
        this->set_pixel_safe(centerX+x,centerY-y,color);
        this->set_pixel_safe(centerX-x,centerY-y,color);
        this->set_pixel_safe(centerX+y,centerY+x,color);
        this->set_pixel_safe(centerX-y,centerY+x,color);
        this->set_pixel_safe(centerX+y,centerY-x,color);
        this->set_pixel_safe(centerX-y,centerY-x,color);
        
      }
      
    }
    //-------------------------------------------------------------------------
    protected:
    template <class TT=T> void fill_color_for_draw_text(const typename std::enable_if<std::is_arithmetic<TT>::value,color_type>::type &color,cimg_underlying_type alternativeValue,cimg_underlying_type (&res)[N<=1?1:N])const
    {
      Color<T,N<=1?1:N> ccolor(color);
      for(std::size_t i=0;i<ccolor.size();i++) res[i]=ccolor[i];
    }
    template <class TT=T> void fill_color_for_draw_text(const typename std::enable_if<!std::is_arithmetic<TT>::value,color_type>::type &color,cimg_underlying_type alternativeValue,cimg_underlying_type (&res)[N<=1?1:N])const
    {
      for(std::size_t i=0;i<color.size();i++) res[i]=alternativeValue;
    }
    
    public:
    //-------------------------------------------------------------------------
    /** Draw text into the image.
     *  If the text is drawn into an empty image this methods reallocates its data such that 
     *  the text is completely visible. A non-empty image is not evaluated and if necessary
     *  the drawn text is cropped instead.
     *  @param offsetZ Left-top corner of the drawn text.
     *  @param text Text to be drawn.
     *  @param size Font size in pixels.
     *  @param foregroundColor Color of the text.
     *  @param backgroundColor Background color of the bounding box of the drawn text.
     */
    void draw_text(const Pixel<int> &offsetZ,const std::string &text,int size,const color_type &foregroundColor,const color_type &backgroundColor)
    {
      this->draw_text(offsetZ.x,offsetZ.y,text,size,foregroundColor,backgroundColor);
    }
    /** Draw text into the image.
     *  If the text is drawn into an empty image this methods reallocates its data such that 
     *  the text is completely visible. A non-empty image is not evaluated and if necessary
     *  the drawn text is cropped instead.
     *  @param text Text to be drawn.
     *  @param size Font size in pixels.
     *  @param foregroundColor Color of the text.
     *  @param backgroundColor Background color of the bounding box of the drawn text.
     */
    void draw_text(const std::string &text,int size,const color_type &foregroundColor,const color_type &backgroundColor)
    {
      this->draw_text(0,0,text,size,foregroundColor,backgroundColor);
    }
    //-------------------------------------------------------------------------
    /** Draw text into the image.
     *  If the text is drawn into an empty image this methods reallocates its data such that 
     *  the text is completely visible. A non-empty image is not evaluated and if necessary
     *  the drawn text is cropped instead.
     *  @param offsetX X coordinate of the left-top corner of the drawn text.
     *  @param offsetY Y coordinate of the left-top corner of the drawn text.
     *  @param text Text to be drawn.
     *  @param size Font size in pixels.
     *  @param foregroundColor Color of the text.
     *  @param backgroundColor Background color of the bounding box of the drawn text.
     */
    void draw_text(int offsetX,int offsetY,const std::string &text,int size,const color_type &foregroundColor,const color_type &backgroundColor)
    {
      int x,y;
      cimg_underlying_type bgc[N<=1?1:N];
      cimg_underlying_type fgc[N<=1?1:N];
      this->fill_color_for_draw_text(backgroundColor,0,bgc);
      this->fill_color_for_draw_text(foregroundColor,255,fgc);
      __PrivateCImgWrapper<cimg_underlying_type> cimg(this->spectrum,text,fgc,bgc,size);
      

      if(this->width==0 && this->height==0) 
      {
        if(offsetX<0) offsetX=0;
        if(offsetY<0) offsetY=0;
        this->reallocate(cimg.width+offsetX,cimg.height+offsetY);
      }
      cimg_color_type cc{};
      color_type color{};
      for(x=0;x<cimg.width;x++)
      {
        for(y=0;y<cimg.height;y++)
        {
          cimg.get(x,y,cc);
          if(std::is_arithmetic<T>::value) color=cc;
          else
          {
            double average=cimg.get_average(x,y);
            color=average>127?foregroundColor:backgroundColor;
          }
          this->set_pixel_safe(x+offsetX,y+offsetY,color);
        }
      }
      
    }
    //-------------------------------------------------------------------------
    /** Implements Matlab function conv2(this,filter,'same').
     *  This method is defined only if the underlying type T of the segmentation/image is arithmetic.
     *  @param filter Convolution filter.
     *  @param boundariesNormalizeBrightness If true, pixels near image boundary are convolved using only 
     *         a "valid" part of the filter, which overlaps with the image. False is equivalent to zero-padding.
     */
    template <class TT=T> void conv2(const typename std::enable_if<std::is_arithmetic<TT>::value,ImageSegmentation<T,N> >::type &filter,bool boundariesNormalizeBrightness=true)
    {
      static_assert(std::is_same<TT,T>::value,"Error [ImageSegmentation::conv2]: Cannot change the underlying type.");
      this->conv2(filter,filter.width/2,filter.height/2,boundariesNormalizeBrightness);
    }
    /** Implements Matlab function conv2(this,filter,'same').
     *  This method is defined only if the underlying type T of the segmentation/image is arithmetic.
     *  @param filter Convolution filter.
     *  @param filterCenterZ Coordinates of the center pixel of the filter. 
     *  @param boundariesNormalizeBrightness If true, pixels near image boundary are convolved using only 
     *         a "valid" part of the filter, which overlaps with the image. False is equivalent to zero-padding.
     */    
    template <class TT=T> void conv2(const typename std::enable_if<std::is_arithmetic<TT>::value,ImageSegmentation<T,N> >::type &filter,const Pixel<int> &filterCenterZ,bool boundariesNormalizeBrightness=true)
    {
      static_assert(std::is_same<TT,T>::value,"Error [ImageSegmentation::conv2]: Cannot change the underlying type.");
      this->conv2(filter,filterCenterZ.x,filterCenterZ.y,boundariesNormalizeBrightness);
    }
    /** Implements Matlab function conv2(this,filter,'same').
     *  This method is defined only if the underlying type T of the segmentation/image is arithmetic.
     *  @param filter Convolution filter.
     *  @param filterCenterX X coordinate of the center pixel of the filter.
     *  @param filterCenterY Y coordinate of the center pixel of the filter.  
     *  @param boundariesNormalizeBrightness If true, pixels near image boundary are convolved using only 
     *         a "valid" part of the filter, which overlaps with the image. False is equivalent to zero-padding.
     */    
    template <class TT=T> void conv2(const typename std::enable_if<std::is_arithmetic<TT>::value,ImageSegmentation<T,N> >::type &filter,int filterCenterX, int filterCenterY,bool boundariesNormalizeBrightness=true)
    {
      static_assert(std::is_same<TT,T>::value,"Error [ImageSegmentation::conv2]: Cannot change the underlying type.");
      int x,y,dx,dy;
      int imageX,imageY;
      double sumFilter=0,sumInside;
      if(boundariesNormalizeBrightness)
        for(x=0;x<filter.width;x++)
          for(y=0;y<filter.height;y++)
            sumFilter+=filter.data[x][y];
      ImageSegmentation<T,N> tmp(this->width,this->height);
      int startDX,endDX,startDY,endDY;
      for(x=0;x<this->width;x++)
      {
        for(y=0;y<this->height;y++)
        {
          tmp.data[x][y]=0;
          sumInside=0;
          startDX=std::max(0,x+filterCenterX-this->width+1);
          startDY=std::max(0,y+filterCenterY-this->height+1);
          endDX=std::min(filter.width-1,x+filterCenterX);
          endDY=std::min(filter.height-1,y+filterCenterY);
          for(dx=startDX;dx<=endDX;dx++)
          {
            for(dy=startDY;dy<=endDY;dy++)
            {
              imageX=x-(dx-filterCenterX);
              imageY=y-(dy-filterCenterY);
              tmp.data[x][y]+=this->data[imageX][imageY]*filter.data[dx][dy];
              sumInside+=filter.data[dx][dy];
            }
          }
          if(boundariesNormalizeBrightness)
          {
            if(sumInside<1e-10) tmp.data[x][y]=this->data[x][y];
            else tmp.data[x][y]*=(sumFilter/sumInside);
          }
        }
      }
      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          this->data[x][y]=tmp.data[x][y];
    }
    //-------------------------------------------------------------------------
    /** Flood fill algorithm.
     *  @param startZ Starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */
    void flood_fill(const Pixel<int> &startZ,
                    const T &targetLabel,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      this->flood_fill(startZ.x,startZ.y,targetLabel,resPixels,overwriteImage,eightNeighborhood);
    }
    /** Flood fill algorithm.
     *  @param startX X coordinate of the starting point.
     *  @param startY Y coordinate of the starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */    
    void flood_fill(int startX,int startY,
                    const T &targetLabel,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      auto equalsFunction=[](const T &c1,const T &c2){return(c1==c2);};
      this->flood_fill(startX,startY,targetLabel,equalsFunction,resPixels,overwriteImage,eightNeighborhood);
    }
    //-------------------------------------------------------------------------
    /** Flood fill algorithm.
     *  @param startZ Starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param boundingBoxZ1 Top-left corner of a rectangle that bounds pixels that might be flooded.
     *  @param boundingBoxZ2 Bottom-right corner of a rectangle that bounds pixels that might be flooded.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */  
    void flood_fill(const Pixel<int> &startZ,
                    const T &targetLabel,
                    const Pixel<int> &boundingBoxZ1,const Pixel<int> &boundingBoxZ2,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      this->flood_fill(startZ.x,startZ.y,targetLabel,boundingBoxZ1,boundingBoxZ2,resPixels,overwriteImage,eightNeighborhood);
    }
    //-------------------------------------------------------------------------
    /** Flood fill algorithm.
     *  @param startX X coordinate of the starting point.
     *  @param startY Y coordinate of the starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param boundingBoxZ1 Top-left corner of a rectangle that bounds pixels that might be flooded.
     *  @param boundingBoxZ2 Bottom-right corner of a rectangle that bounds pixels that might be flooded.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */ 
    void flood_fill(int startX,int startY,
                    const T &targetLabel,
                    const Pixel<int> &boundingBoxZ1,const Pixel<int> &boundingBoxZ2,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      auto equalsFunction=[](const T &c1,const T &c2){return(c1==c2);};
      this->flood_fill(startX,startY,targetLabel,boundingBoxZ1,boundingBoxZ2,equalsFunction,resPixels,overwriteImage,eightNeighborhood);
    }
    
    
    
    //-------------------------------------------------------------------------
    /** Flood fill algorithm.
     *  @param startZ Starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param equalsFunction A callable with two arguments of type const ImageSegmentation<T,N>::color_type &, 
     *         that returns true if its parameters are considered equal and false otherwise.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */
    template <class EqualsFunctionType> 
    void flood_fill(const Pixel<int> &startZ,
                    const T &targetLabel,
                    EqualsFunctionType equalsFunction,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      this->flood_fill(startZ.x,startZ.y,targetLabel,equalsFunction,resPixels,overwriteImage,eightNeighborhood);
    }
    /** Flood fill algorithm.
     *  @param startX X coordinate of the starting point.
     *  @param startY Y coordinate of the starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param equalsFunction A callable with two arguments of type const ImageSegmentation<T,N>::color_type &, 
     *         that returns true if its parameters are considered equal and false otherwise.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */
    template <class EqualsFunctionType> 
    void flood_fill(int startX,int startY,
                    const T &targetLabel,
                    EqualsFunctionType equalsFunction,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      this->flood_fill(startX,startY,targetLabel,Pixel<int>(0,0),Pixel<int>(this->width-1,this->height-1),equalsFunction,resPixels,overwriteImage,eightNeighborhood);
    }
    //-------------------------------------------------------------------------
    /** Flood fill algorithm.
     *  @param startZ Starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param boundingBoxZ1 Top-left corner of a rectangle that bounds pixels that might be flooded.
     *  @param boundingBoxZ2 Bottom-right corner of a rectangle that bounds pixels that might be flooded.
     *  @param equalsFunction A callable with two arguments of type const ImageSegmentation<T,N>::color_type &, 
     *         that returns true if its parameters are considered equal and false otherwise.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */    
    template <class EqualsFunctionType> 
    void flood_fill(const Pixel<int> &startZ,
                    const T &targetLabel,
                    const Pixel<int> &boundingBoxZ1,const Pixel<int> &boundingBoxZ2,
                    EqualsFunctionType equalsFunction,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      this->flood_fill(startZ.x,startZ.y,targetLabel,boundingBoxZ1,boundingBoxZ2,equalsFunction,resPixels,overwriteImage,eightNeighborhood);
    }
    /** Flood fill algorithm.
     *  @param startX X coordinate of the starting point.
     *  @param startY Y coordinate of the starting point.
     *  @param targetLabel If overwriteImage==true the color of flooded pixels will be set to _targetLabel_.
     *  @param boundingBoxZ1 Top-left corner of a rectangle that bounds pixels that might be flooded.
     *  @param boundingBoxZ2 Bottom-right corner of a rectangle that bounds pixels that might be flooded.
     *  @param equalsFunction A callable with two arguments of type const ImageSegmentation<T,N>::color_type &, 
     *         that returns true if its parameters are considered equal and false otherwise.
     *  @param resPixels std::vector of flooded pixels.
     *  @param overwriteImage Determines, whether set the color of flooded pixels to _targetLabel_.
     *  @param eightNeighborhood If true, the flooding considers 8-neigborhood of each pixel, false means 4-neigborhood.
     */    
    template <class EqualsFunctionType> 
    void flood_fill(int startX,int startY,
                    const T &targetLabel,
                    const Pixel<int> &boundingBoxZ1,const Pixel<int> &boundingBoxZ2,
                    EqualsFunctionType equalsFunction,
                    std::vector<Pixel<int> > &resPixels,
                    bool overwriteImage=true,
                    bool eightNeighborhood=false)
    {
      resPixels.clear();
      if(this->is_pixel_inside(startX,startY,boundingBoxZ1,boundingBoxZ2) && !equalsFunction(this->data[startX][startY],targetLabel))
      {
        T sourceLabel=this->data[startX][startY];
        std::list<Pixel<int> > pixels;
        std::list<PixelValue<T> > cleanPixels;
        auto add_pixel=[&](int x,int y){pixels.emplace_back(x,y);cleanPixels.emplace_back(x,y,this->data[x][y]);this->data[x][y]=targetLabel;};
        
        add_pixel(startX,startY);
        while(!pixels.empty())
        {
          auto &&p=*pixels.begin();
          resPixels.push_back(p);
          if(p.x>boundingBoxZ1.x && equalsFunction(this->data[p.x-1][p.y],sourceLabel)) add_pixel(p.x-1,p.y);
          if(p.y>boundingBoxZ1.y && equalsFunction(this->data[p.x][p.y-1],sourceLabel)) add_pixel(p.x,p.y-1);
          if(p.x<boundingBoxZ2.x && equalsFunction(this->data[p.x+1][p.y],sourceLabel)) add_pixel(p.x+1,p.y);
          if(p.y<boundingBoxZ2.y && equalsFunction(this->data[p.x][p.y+1],sourceLabel)) add_pixel(p.x,p.y+1);
          if(eightNeighborhood)
          {
            if(p.x>boundingBoxZ1.x && p.y>boundingBoxZ1.y && equalsFunction(this->data[p.x-1][p.y-1],sourceLabel)) add_pixel(p.x-1,p.y-1);
            if(p.x>boundingBoxZ1.x && p.y<boundingBoxZ2.y && equalsFunction(this->data[p.x-1][p.y+1],sourceLabel)) add_pixel(p.x-1,p.y+1);
            
            if(p.x<boundingBoxZ2.x && p.y>boundingBoxZ1.y && equalsFunction(this->data[p.x+1][p.y-1],sourceLabel)) add_pixel(p.x+1,p.y-1);
            if(p.x<boundingBoxZ2.x && p.y<boundingBoxZ2.y && equalsFunction(this->data[p.x+1][p.y+1],sourceLabel)) add_pixel(p.x+1,p.y+1);
          }
          pixels.pop_front();
        }
        for(auto &&p: cleanPixels) this->data[p.x][p.y]=p.value;
        if(overwriteImage)
          for(auto &&p: resPixels) 
            this->data[p.x][p.y]=targetLabel;
      }      
 
    }

    //-------------------------------------------------------------------------
    /*GrayScale: (optional: uniform}
      load all channels, average
    GrayFloatingPoint: (optional: uniform, bitdepth}
      load all channels, average/255
    SegmentationBlackWhite (optional: bitdepth}
      load all channels, average, normalize
    Segmentation (optional: palette}
      load all channels, if the image is 1 channel, use directly, if equal palette, get_color_index, else throw exception
    RGB
      load all channels, if the image is 1 channel, repeat, if more, replace missing by zeros*/

    /** Load segmentation from file using the standard palette.
     */
    template <int NN=N> 
    void load(const typename std::enable_if<NN==DefaultTypes::SegmentationN,std::string>::type &filename)
    {
      static_assert(NN==N,"Error [ImageSegmentation::load]: Cannot change the number of channels.");
      StandardPalette sp;
      this->load(filename,sp);
    }
    //-------------------------------------------------------------------------
    /** Load segmentation from file using a custom palette.
     */
    template <class PT,std::size_t PN,int NN=N> 
    void load(const typename std::enable_if<NN==DefaultTypes::SegmentationN,std::string>::type &filename,const Palette<PT,PN> &palette)
    {
      static_assert(NN==N,"Error [ImageSegmentation::load]: Cannot change the number of channels.");
      int x,y;
      __PrivateCImgWrapper<DefaultTypes::int_type> cimg(filename);
      this->reallocate(cimg.width,cimg.height);
      if(cimg.spectrum==1)
      {
        for(x=0;x<this->width;x++)
          for(y=0;y<this->height;y++)
            cimg.get(x,y,this->data[x][y]);
        if(this->verbose>=Verbose::Normal) std::cerr << compose_message(Message::Note,"ImageSegmentation::load","1 channel segmentation loaded correctly from "+filename) << "\n";
      }
      else if(cimg.spectrum==palette.spectrum)
      {
        std::vector<int> color;
        for(x=0;x<this->width;x++)
        {
          for(y=0;y<this->height;y++)
          {
            cimg.get(x,y,color);
            this->data[x][y]=palette.get_color_index(color);
          }
        }
        if(this->verbose>=Verbose::Normal)
        {
          int numofNR=this->count_label(palette.colorNotRecognized);
          if(numofNR>0) 
          {
            std::cerr << compose_message(Message::Warning,
                                                     "ImageSegmentation::load",
                                                     std::to_string(palette.spectrum)+" channel segmentation loaded from "+filename+" but there are "+
                                                     std::to_string(numofNR)+" pixels with unrecognized labels.") << "\n";
          }
          else std::cerr << compose_message(Message::Note,"ImageSegmentation::load",std::to_string(palette.spectrum)+" channel segmentation loaded correctly from "+filename) << "\n";
        }
      }
      else
      {
        throw(BadSegmentationFormatException(palette.spectrum,cimg.spectrum,filename));
      }
    }
    //-------------------------------------------------------------------------
    protected:
    double bitdepth_2_norm(int bitdepth)const {if(bitdepth<0) {bitdepth+=9;}return((1<<bitdepth)-1);}
    //-------------------------------------------------------------------------
    public:
    /** Load binary (black&white) segmentation from file.
     */
    template <int NN=N> void load(const typename std::enable_if<NN==DefaultTypes::SegmentationBlackWhiteN,std::string>::type &filename,int bitdepth=-1)
    {
      static_assert(NN==N,"Error [ImageSegmentation::load]: Cannot change the number of channels.");
      int x,y;
      double middle=this->bitdepth_2_norm(bitdepth-1)+1;
      __PrivateCImgWrapper<DefaultTypes::int_type> cimg(filename);
      this->reallocate(cimg.width,cimg.height);
      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          this->data[x][y]=cimg.get_average(x,y)>=middle;
      if(this->verbose>=Verbose::Normal) std::cerr << compose_message(Message::Note,"ImageSegmentation::load","Black&white segmentation loaded correctly from "+filename) << "\n";
    }
    //-------------------------------------------------------------------------
    /** Load multichannel image.
     *  If _filename_ contains an image with number of channels different from template parameter N,
     *  the colors of individual pixels are loaded as follows:
     *  
     * 1 channel in _filename_, N>1: All channels are set to the value of the first channel in _filename_.
     * 
     * _filename_ has less channels than N: All channels of _filename_ are loaded and the remaining are set to 0.
     *
     * _filename_ has more channels than N: Channels 1-N are loaded from _filename_ and the remaining channels in _filename_ are ignored.
     */
    template <int NN=N> void load(const typename std::enable_if<1<NN,std::string>::type &filename)
    {
      static_assert(NN==N,"Error [ImageSegmentation::load]: Cannot change the number of channels.");
      int x,y;
      __PrivateCImgWrapper<DefaultTypes::int_type> cimg(filename);
      this->reallocate(cimg.width,cimg.height);
      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          cimg.get(x,y,this->data[x][y]);
      if(this->verbose>=Verbose::Normal) std::cerr << compose_message(Message::Note,
                                                                      "ImageSegmentation::load",
                                                                      std::to_string(N)+
                                                                      " channel image loaded correctly from "+filename) << "\n";
    }
    //-------------------------------------------------------------------------
    /** Load a single channel image from file.
     *  @param filename Path to the loaded file.
     *  @param averageIfRGB If false and the input image has three channels (RGB), gray value of each pixel
     *                      is calculated as 0.2989*R+0.5871*G+0.1140*B instead of simple average.
     *  @param bitdepth Expected bitdepth of the file. Valid values are -1 (default), 8, 16, 32 and 64.
     *                  Value -1 means, that the bitdepth is estimated automatically. This argument is only useful
     *                  for images with floating point underlying type, which are normalized to \[0,1\] interval.
     */     
    template <int NN=N> 
    void load(const typename std::enable_if<!(NN==DefaultTypes::SegmentationN || NN==DefaultTypes::SegmentationBlackWhiteN || 1<NN),std::string>::type &filename,bool averageIfRGB=false,int bitdepth=-1)
    {
      static_assert(NN==N,"Error [ImageSegmentation::load]: Cannot change the number of channels.");
      auto average=[](const std::vector<int> &color)->T
      {
        T ret=0;
        for(auto &&it: color){ret+=it;}
        if(color.size()>0)ret/=(T)color.size();
        return(ret);
      };
      int x,y;
      __PrivateCImgWrapper<DefaultTypes::int_type> cimg(filename);
      this->reallocate(cimg.width,cimg.height);
      std::vector<int> color;
      
      if(N==3 && !averageIfRGB)
      {
        for(x=0;x<this->width;x++)
        {
          for(y=0;y<this->height;y++)
          {
            cimg.get(x,y,color);
            this->data[x][y]=(double)color[0]*0.2989+(double)color[1]*0.5871+(double)color[2]*0.1140;
          }
        }
      }
      else
      {
        for(x=0;x<this->width;x++)
        {
          for(y=0;y<this->height;y++)
          {
            cimg.get(x,y,color);
            this->data[x][y]=average(color);
          }
        }
      }
      if(std::is_floating_point<T>::value)
      {
        double norm=this->bitdepth_2_norm(bitdepth);
        for(x=0;x<this->width;x++)
          for(y=0;y<this->height;y++)
            this->data[x][y]/=norm;
      }
      if(this->verbose>=Verbose::Normal) 
      {
        std::string typeName="";
        if(std::is_integral<T>::value) typeName=" integral ";
        if(std::is_floating_point<T>::value) typeName=" floating point ";
        std::cerr << compose_message(Message::Note,"ImageSegmentation::load","1 channel"+typeName+"image loaded correctly from "+filename) << "\n";
      }

    }
    protected:
    //-------------------------------------------------------------------------
    int count_nonwhite_characters(const std::string &str)const
    {
      int res=0;
      for(auto &&ch: str)
        if(ch>(char)32) 
          res++;
      return(res);
    }
    template <class TT=T> T atot(const typename std::enable_if<std::is_integral<TT>::value,std::string>::type &str)const{return(atoi(str.c_str()));}
    template <class TT=T> T atot(const typename std::enable_if<std::is_floating_point<TT>::value,std::string>::type &str)const{return(atof(str.c_str()));}
    template <class TT=T> T atot(const typename std::enable_if<!(std::is_integral<TT>::value ||  std::is_floating_point<TT>::value),std::string>::type &str)const {return(T(str));}
    public:
    //-------------------------------------------------------------------------
    /** Load CSV file
     *  This function is defined only for segmentations and single channel images.
     */
    template <int NN=N> 
    void load_csv(const typename std::enable_if<NN<=1,std::string>::type &filename,char delimiter=',',bool ignoreBlankLines=true)
    {
      static_assert(NN==N,"Error [ImageSegmentation::load_csv]: Cannot change the number of channels.");
      std::string line,item;
      int lineNum=0;
      std::ifstream in(filename);
      int height=0,width=-1;
      std::vector<int> nonwhite;
      bool ret=true;
      if(in.good()) 
      {
        while(in.good())
        {
          std::getline (in, line);
          std::stringstream ss(line);

          nonwhite.clear();
          lineNum++;
          while(std::getline(ss, item, delimiter)) nonwhite.push_back(this->count_nonwhite_characters(item));
          if((nonwhite.size()==1 && nonwhite[0]==0) || (nonwhite.size()==0 && in.good())) //blank line
          {
            if(ignoreBlankLines) nonwhite.clear();
            else 
            {
              throw(BadCSVFormatException(compose_message(Message::Error,
                                                          "ImageSegmentation::load_csv",
                                                          "File " + filename + ", line " + std::to_string(lineNum) + " is blank.")));
            }
          }
          
          if(width==-1 && nonwhite.size()>0) width=nonwhite.size();
          if(nonwhite.size()>0 && (int)nonwhite.size()!=width)
          {
            throw(BadCSVFormatException(compose_message(Message::Error,
                                                        "ImageSegmentation::load_csv",
                                                        "File " + filename + ", line " + std::to_string(lineNum) + 
                                                        " has different number of items than previous lines ("+
                                                        std::to_string(nonwhite.size()) + "!=" + std::to_string(height)+").")));
          } 
          if(nonwhite.size()>0) height++;
        }
        if(width<0) width=0;
        if(width==0 || height==0)
        {
          throw(BadCSVFormatException(compose_message(Message::Error,
                                                      "ImageSegmentation::load_csv",
                                                      "File " + filename + " contains no csv data.")));
        }
        int currX=0,currY=0;
        if(ret)
        {
          this->reallocate(width,height);
          in.close();
          in.open(filename);
          while(in.good())
          {
            std::getline (in, line);
            std::stringstream ss(line);
            currX=0;
            while (std::getline(ss, item, delimiter))
            {
              this->data[currX][currY]=this->atot<T>(item);
              currX++;
            }
            if(currX==this->width) currY++;
          }
        }
        in.close();
      }
      else
      {
        throw(CoreException(compose_message(Message::Error,"ImageSegmentation::load_csv","File not found: "+filename)));
      }
    }
    //-------------------------------------------------------------------------
    /*
    Segmentation
      save
      save_segmentation
      save_bw
      save_raw
    SegmentationBW
      save
      save_segmentation
      save_bw
      save_raw
    1channel
      save
      save_segmentation
      save_bw
      save_raw
    multi channel
      save
      save_bw
      save_raw

    */
    
    
    protected:

    //-------------------------------------------------------------------------
    template <class U,int NN=N> void _save_raw(const typename std::enable_if<1<NN,std::string>::type &filename,double scale,double m)const
    {
      __PrivateCImgWrapper<U> cimg(this->width,this->height,this->spectrum);
      int x,y,c;
      for(c=0;c<this->spectrum;c++)
        for(x=0;x<this->width;x++)
          for(y=0;y<this->height;y++)
            cimg.set(x,y,c,(U)(m*this->data[x][y][c]));
      cimg.resize(scale);
      cimg.save(filename);
    }
    //-------------------------------------------------------------------------
    template <class U,int NN=N> void _save_raw(const typename std::enable_if<!(1<NN),std::string>::type &filename,double scale,double m)const
    {
      __PrivateCImgWrapper<U> cimg(this->width,this->height,this->spectrum);
      int x,y;
      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          cimg.set(x,y,0,(U)(m*this->data[x][y]));
      cimg.resize(scale);
      cimg.save(filename);
    }
       
    public:
    //-------------------------------------------------------------------------
    /** Save segmentation/image into a file as-is.
     */
    void save_raw(const std::string &filename,double scale=1,int desiredBitdepth=-1)const
    {
      double m=1;
      if(std::is_floating_point<T>::value) m=this->bitdepth_2_norm(desiredBitdepth);
      if(desiredBitdepth==8) this->_save_raw<uint8_t>(filename,scale,m);
      else if(desiredBitdepth==16) this->_save_raw<uint16_t>(filename,scale,m);
      else if(desiredBitdepth==32) this->_save_raw<uint32_t>(filename,scale,m);
      else if(desiredBitdepth==64) this->_save_raw<uint64_t>(filename,scale,m);
      else if(std::is_floating_point<T>::value) this->_save_raw<DefaultTypes::float_type>(filename,scale,m);
      else if(std::is_unsigned<T>::value) this->_save_raw<DefaultTypes::uint_type>(filename,scale,m);
      else this->_save_raw<DefaultTypes::int_type>(filename,scale,m);
    }
    //-------------------------------------------------------------------------
    /** Save binary segmentation into a file.
     * This method is only defined if the underlying type T is arithmetic and assumes, that the foreground is denoted by label 1.
     */
    template <class TT=T,int NN=N> 
    void save_bw(const typename std::enable_if<std::is_arithmetic<TT>::value && (NN==DefaultTypes::SegmentationN || NN==DefaultTypes::SegmentationBlackWhiteN || NN==1),std::string>::type &filename)const
    {
      static_assert(NN==N,"Error [ImageSegmentation::save_bw]: Cannot change the number of channels.");
      static_assert(std::is_same<TT,T>::value,"Error [ImageSegmentation::save_bw]: Cannot change the underlying type.");
      this->save_bw(filename,1);
    }
    //-------------------------------------------------------------------------
    /** Save binary segmentation with custom foreground label into a file.
     */    
    void save_bw(const std::string &filename,const color_type &foregroundColor)const
    {
      __PrivateCImgWrapper<DefaultTypes::int_type> cimg(this->width,this->height,1);
      int x,y;
      for(x=0;x<this->width;x++)
        for(y=0;y<this->height;y++)
          cimg.set(x,y,0,255*(this->data[x][y]==foregroundColor));
      cimg.save(filename);
    }
    //-------------------------------------------------------------------------
    /** Save segmentation into a file using the standard palette.
     */
    template <int NN=N> 
    void save_segmentation(const typename std::enable_if<NN<=1,std::string>::type &filename)const
    {
      static_assert(NN==N,"Error [ImageSegmentation::save_segmentation]: Cannot change the number of channels.");
      StandardPalette sp;
      this->save_segmentation(filename,sp);
    }
    //-------------------------------------------------------------------------
    /** Save segmentation into a file using a custom palette.
     */    
    template <class PT,std::size_t PN,int NN=N> 
    void save_segmentation(const typename std::enable_if<NN<=1,std::string>::type &filename,
                           const Palette<PT,PN> &palette)const
    {
      __PrivateCImgWrapper<DefaultTypes::int_type> cimg(this->width,this->height,palette.spectrum);
      int x,y;
      for(x=0;x<this->width;x++)
      {
        for(y=0;y<this->height;y++)
        {
          auto &&color=palette.get_color_for_segmentation(this->data[x][y]);
          for(int c=0;c<palette.spectrum;c++) cimg.set(x,y,c,color[c]);
        }
      }
      cimg.save(filename);
    }    
    //-------------------------------------------------------------------------
    /** Save segmentation into a file using the standard palette.
     */
    template <int NN=N> void save(const typename std::enable_if<NN==DefaultTypes::SegmentationN,std::string>::type &filename)const
    {
      static_assert(NN==N,"Error [ImageSegmentation::save]: Cannot change the number of channels.");
      StandardPalette sp;
      this->save_segmentation(filename,sp);
    }
    //-------------------------------------------------------------------------
    /** Save segmentation into a file using a custom palette.
     */
    template <class PT,std::size_t PN,int NN=N> 
    void save(const typename std::enable_if<NN==DefaultTypes::SegmentationN,std::string>::type &filename,const Palette<PT,PN> &palette)const
    {
      static_assert(NN==N,"Error [ImageSegmentation::save]: Cannot change the number of channels.");
      this->save_segmentation(filename,palette);
    }
    
    //-------------------------------------------------------------------------
    /** Save binary segmentation into a file.
     */
    template <int NN=N> void save(const typename std::enable_if<NN==DefaultTypes::SegmentationBlackWhiteN,std::string>::type &filename)const
    {
      static_assert(NN==N,"Error [ImageSegmentation::save]: Cannot change the number of channels.");
      this->save_bw(filename,1);
    }
    //-------------------------------------------------------------------------
    //Multichannel and one channel image save
    /** Save single or multichannel image into a file.
     */
    template <int NN=N> 
    void save(const typename std::enable_if<1<=NN && !(NN==DefaultTypes::SegmentationN || NN==DefaultTypes::SegmentationBlackWhiteN),std::string>::type &filename,
              double scale=1,int desiredBitdepth=-1)const
    {
      static_assert(NN==N,"Error [ImageSegmentation::save]: Cannot change the number of channels.");
      this->save_raw(filename,scale,desiredBitdepth);
    }

    //-------------------------------------------------------------------------
    /** Save this segmentation/image into a text csv file.
     */
    void save_csv(const std::string &filename,char delimiter=',')const    
    {
      std::ofstream of(filename);
      if(of.is_open())
      {
        int x,y;
        for(y=0;y<this->height;y++)
        {
          for(x=0;x<this->width;x++)
          {
            of << this->data[x][y];
            if(x<this->width-1) of << delimiter;
          }
          of << "\n";
        }
      }
      of.close();
    }
  };
//-----------------------------------------------------------------------------
  ///Typename for multilabel segmentation.
  using Segmentation=ImageSegmentation<DefaultTypes::int_type,DefaultTypes::SegmentationN>;
  ///Typename for binary segmentation.
  using SegmentationBW=ImageSegmentation<DefaultTypes::int_type,DefaultTypes::SegmentationBlackWhiteN>;
  ///Typename for RGB image.
  using ImageRGB=ImageSegmentation<DefaultTypes::int_type,3>;
  ///Typename for RGB image.
  using RGBImage=ImageRGB;
  ///Typename for one channel image.
  template <class T> using Image=ImageSegmentation<T,1>;
//-----------------------------------------------------------------------------
  /** Load MNIST dataset (segmentations) from file 
   *  @param imagesFilename Path to the file with images. If _imagesFilename_=="", the images are not loaded.
   *  @param labelsFilename Path to the file with class labels. If _labelsFilename_=="", the class labels are not loaded.
   *  @param resImages std::vector with loaded segmentations.
   *  @param resLabels std::vector with loaded class labels.
   *  @param threshold A threshold between background and foreground.
   *  @param indices Indices of segmentations and/or labels to be loaded.
   */  
  template <int N> void load_mnist(const std::string &imagesFilename,
                                   const std::string &labelsFilename,
                                   std::vector<ImageSegmentation<DefaultTypes::int_type,N> > &resImages,
                                   std::vector<int> &resLabels,
                                   int threshold,
                                   std::vector<int> *indices=nullptr);
  /** Load MNIST dataset (single channel images) from file.
   *  @param imagesFilename Path to the file with images. If _imagesFilename_=="", the images are not loaded.
   *  @param labelsFilename Path to the file with class labels. If _labelsFilename_=="", the class labels are not loaded.
   *  @param resImages std::vector with loaded images.
   *  @param resLabels std::vector with loaded class labels.
   *  @param indices Indices of images and/or labels to be loaded.
   */  
  void load_mnist(const std::string &imagesFilename,
                  const std::string &labelsFilename,
                  std::vector<Image<DefaultTypes::int_type> > &resImages,
                  std::vector<int> &resLabels,
                  std::vector<int> *indices=nullptr);

}
#endif


