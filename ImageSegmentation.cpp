#include <CImg.h>
#include "ImageSegmentation.h"
LibImageSegmentation::BadSegmentationFormatException::BadSegmentationFormatException(int paletteSpectrum,int cimgSpectrum,const std::string &filename) 
{
  this->message=std::string("Unexpected number of channels, expected 1 or ")+
                                       std::to_string(paletteSpectrum)+
                                       ", got "+std::to_string(cimgSpectrum)+" in "+filename;
}
//-----------------------------------------------------------------------------
LibImageSegmentation::StandardPalette::StandardPalette()
{
  int i,j,k;
  double m;
  this->push_back({0,0,0});
  this->push_back({255,0,0});this->push_back({0,255,0});this->push_back({0,0,255});
  this->push_back({255,255,0});this->push_back({255,0,255});this->push_back({0,255,255});
  this->push_back({255,255,255});
  for(j=2;j<=8;j*=2)
  {
    for(k=1;k<j;k+=2)
    {
      m=(double)k/(double)j;
      for(i=1;i<8;i++) this->push_back({(int)(this->at(i)[0]*m),(int)(this->at(i)[1]*m),(int)(this->at(i)[2]*m)});
    }
  }
}
//-----------------------------------------------------------------------------
LibImageSegmentation::BrightPalette::BrightPalette()
{
  this->push_back({0,0,0});
  this->push_back({255,0,0});this->push_back({0,255,0});this->push_back({0,0,255});
  this->push_back({255,255,0});this->push_back({255,0,255});this->push_back({0,255,255});
  this->push_back({255,255,255});
  this->push_back({255,127,0});this->push_back({255,0,127});this->push_back({0,255,127});
  this->push_back({127,255,0});this->push_back({127,0,255});this->push_back({0,127,255});
  this->push_back({255,127,127});this->push_back({127,255,127});this->push_back({127,127,255});
  this->push_back({255,255,127});this->push_back({255,127,255});this->push_back({127,255,255});
  this->push_back({191,191,191});
}
//-----------------------------------------------------------------------------
#define INST_PCIW_CONSTRUCTOR_DEFAULT(header) template LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::int_type>::header;\
                                              template LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::uint_type>::header;\
                                              template LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::float_type>::header

#define INST_PCIW_CONSTRUCTOR(datatype,header,dummynumber) template std::conditional<\
                                                              std::is_same<datatype, LibImageSegmentation::DefaultTypes::int_type>::value || \
                                                              std::is_same<datatype, LibImageSegmentation::DefaultTypes::uint_type>::value || \
                                                              std::is_same<datatype, LibImageSegmentation::DefaultTypes::float_type>::value,\
                                                              __Dummy::__PrivateCImgWrapper<datatype,dummynumber>,\
                                                              LibImageSegmentation::__PrivateCImgWrapper<datatype> >::type::header

#define INST_PCIW_METHOD_DEFAULT(returntype,header) template returntype LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::int_type>::header;\
                                                    template returntype LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::uint_type>::header;\
                                                    template returntype LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::float_type>::header

#define INST_PCIW_METHOD(returntype,datatype,header,dummynumber) template returntype std::conditional<\
                                                              std::is_same<datatype, LibImageSegmentation::DefaultTypes::int_type>::value || \
                                                              std::is_same<datatype, LibImageSegmentation::DefaultTypes::uint_type>::value || \
                                                              std::is_same<datatype, LibImageSegmentation::DefaultTypes::float_type>::value,\
                                                              __Dummy::__PrivateCImgWrapper<datatype,dummynumber>,\
                                                              LibImageSegmentation::__PrivateCImgWrapper<datatype> >::type::header

namespace __Dummy
{
  template <class T,int N> class __PrivateCImgWrapper
  {
    __PrivateCImgWrapper(const std::string &filename){}
    __PrivateCImgWrapper(int width,int height,int spectrum){}
    __PrivateCImgWrapper(int spectrum,const std::string &text,T *foregroundColor,T *backgroundColor,int size){}
    ~__PrivateCImgWrapper(){}
    T _get(int x,int y,int c)const{return(T{});}
    void get(int x,int y,std::vector<T> &res)const{}
    double get_average(int x,int y)const{return(0);}
    void set(int x,int y,int c,T value){}
    void resize(double m,int cimgInterpolationType=1){}
    void save(const std::string &filename)const{}
  };
}

//-----------------------------------------------------------------------------
template <class T> LibImageSegmentation::__PrivateCImgWrapper<T>::__PrivateCImgWrapper(const std::string &filename)
{
  try
  {
    this->cimg=new cimg_library::CImg<T>(filename.c_str());
    auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
    this->init_dimensions(d.width(),d.height(),d.spectrum());
  }
  catch(const cimg_library::CImgIOException &e)
  {
    throw(CoreException(e.what()));
  }
}
INST_PCIW_CONSTRUCTOR_DEFAULT(__PrivateCImgWrapper(const std::string &filename));
INST_PCIW_CONSTRUCTOR(uint8_t,__PrivateCImgWrapper(const std::string &filename),__LINE__);
INST_PCIW_CONSTRUCTOR(uint16_t,__PrivateCImgWrapper(const std::string &filename),__LINE__);
INST_PCIW_CONSTRUCTOR(uint32_t,__PrivateCImgWrapper(const std::string &filename),__LINE__);
INST_PCIW_CONSTRUCTOR(uint64_t,__PrivateCImgWrapper(const std::string &filename),__LINE__);

//-----------------------------------------------------------------------------
template <class T> LibImageSegmentation::__PrivateCImgWrapper<T>::__PrivateCImgWrapper(int width,int height,int spectrum)
{
  this->cimg=new cimg_library::CImg<T>(width,height,1,spectrum);
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  this->init_dimensions(d.width(),d.height(),d.spectrum());
  
}
INST_PCIW_CONSTRUCTOR_DEFAULT(__PrivateCImgWrapper(int width,int height,int spectrum));
INST_PCIW_CONSTRUCTOR(uint8_t,__PrivateCImgWrapper(int width,int height,int spectrum),__LINE__);
INST_PCIW_CONSTRUCTOR(uint16_t,__PrivateCImgWrapper(int width,int height,int spectrum),__LINE__);
INST_PCIW_CONSTRUCTOR(uint32_t,__PrivateCImgWrapper(int width,int height,int spectrum),__LINE__);
INST_PCIW_CONSTRUCTOR(uint64_t,__PrivateCImgWrapper(int width,int height,int spectrum),__LINE__);

//-----------------------------------------------------------------------------
template <class T> LibImageSegmentation::__PrivateCImgWrapper<T>::__PrivateCImgWrapper(int spectrum,const std::string &text,T *foregroundColor,T *backgroundColor,int size)
{
  cimg_library::CImg<unsigned char> dimensions;
  unsigned char c=1;
  dimensions.draw_text(0,0,text.c_str(),&c,&c,1,size);
  this->cimg=new cimg_library::CImg<T>(dimensions.width(),dimensions.height(),1,spectrum);
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  d.draw_text(0,0,text.c_str(),foregroundColor,backgroundColor,1,size);
  this->init_dimensions(d.width(),d.height(),d.spectrum());
}
template LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::int_type>::__PrivateCImgWrapper(int spectrum,const std::string &text,DefaultTypes::int_type *foregroundColor,DefaultTypes::int_type *backgroundColor,int size);
template LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::uint_type>::__PrivateCImgWrapper(int spectrum,const std::string &text,DefaultTypes::uint_type *foregroundColor,DefaultTypes::uint_type *backgroundColor,int size);
template LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::float_type>::__PrivateCImgWrapper(int spectrum,const std::string &text,DefaultTypes::float_type *foregroundColor,DefaultTypes::float_type *backgroundColor,int size);
INST_PCIW_CONSTRUCTOR(uint8_t,__PrivateCImgWrapper(int spectrum,const std::string &text,uint8_t *foregroundColor,uint8_t *backgroundColor,int size),__LINE__);
INST_PCIW_CONSTRUCTOR(uint16_t,__PrivateCImgWrapper(int spectrum,const std::string &text,uint16_t *foregroundColor,uint16_t *backgroundColor,int size),__LINE__);
INST_PCIW_CONSTRUCTOR(uint32_t,__PrivateCImgWrapper(int spectrum,const std::string &text,uint32_t *foregroundColor,uint32_t *backgroundColor,int size),__LINE__);
INST_PCIW_CONSTRUCTOR(uint64_t,__PrivateCImgWrapper(int spectrum,const std::string &text,uint64_t *foregroundColor,uint64_t *backgroundColor,int size),__LINE__);

//-----------------------------------------------------------------------------
template <class T> LibImageSegmentation::__PrivateCImgWrapper<T>::~__PrivateCImgWrapper()
{
  auto d=static_cast<cimg_library::CImg<T> *>(this->cimg);
  delete d;
  this->cimg=nullptr;
}
INST_PCIW_CONSTRUCTOR_DEFAULT(~__PrivateCImgWrapper());
INST_PCIW_CONSTRUCTOR(uint8_t,~__PrivateCImgWrapper(),__LINE__);
INST_PCIW_CONSTRUCTOR(uint16_t,~__PrivateCImgWrapper(),__LINE__);
INST_PCIW_CONSTRUCTOR(uint32_t,~__PrivateCImgWrapper(),__LINE__);
INST_PCIW_CONSTRUCTOR(uint64_t,~__PrivateCImgWrapper(),__LINE__);


//-----------------------------------------------------------------------------
template <class T> T LibImageSegmentation::__PrivateCImgWrapper<T>::_get(int x,int y,int c)const
{
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  T ret=0;
  if(d.spectrum()==1) ret=d(x,y,0,0);
  else if(c<d.spectrum()) ret=d(x,y,0,c);
  return(ret);
}
template LibImageSegmentation::DefaultTypes::int_type LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::int_type>::_get(int x,int y,int c)const;
template LibImageSegmentation::DefaultTypes::uint_type LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::uint_type>::_get(int x,int y,int c)const;
template LibImageSegmentation::DefaultTypes::float_type LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::float_type>::_get(int x,int y,int c)const;
INST_PCIW_METHOD(uint8_t,uint8_t,_get(int x,int y,int c)const,__LINE__);
INST_PCIW_METHOD(uint16_t,uint16_t,_get(int x,int y,int c)const,__LINE__);
INST_PCIW_METHOD(uint32_t,uint32_t,_get(int x,int y,int c)const,__LINE__);
INST_PCIW_METHOD(uint64_t,uint64_t,_get(int x,int y,int c)const,__LINE__);


//-----------------------------------------------------------------------------
template <class T> double LibImageSegmentation::__PrivateCImgWrapper<T>::get_average(int x,int y)const
{
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  double ret=0;
  int n=0;
  for(auto c=0;c<d.spectrum();c++) {ret+=d(x,y,0,c);n++;}
  if(n>0) ret/=(double)n;
  return(ret);
}
INST_PCIW_METHOD_DEFAULT(double,get_average(int x,int y)const);
INST_PCIW_METHOD(double,uint8_t,get_average(int x,int y)const,__LINE__);
INST_PCIW_METHOD(double,uint16_t,get_average(int x,int y)const,__LINE__);
INST_PCIW_METHOD(double,uint32_t,get_average(int x,int y)const,__LINE__);
INST_PCIW_METHOD(double,uint64_t,get_average(int x,int y)const,__LINE__);

//-----------------------------------------------------------------------------
template <class T> void LibImageSegmentation::__PrivateCImgWrapper<T>::set(int x,int y,int c,T value)
{
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  d(x,y,0,c)=value;
}
template void LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::int_type>::set(int x,int y,int c,DefaultTypes::int_type value);
template void LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::uint_type>::set(int x,int y,int c,DefaultTypes::uint_type value);
template void LibImageSegmentation::__PrivateCImgWrapper<LibImageSegmentation::DefaultTypes::float_type>::set(int x,int y,int c,DefaultTypes::float_type value);
INST_PCIW_METHOD(void,uint8_t,set(int x,int y,int c,uint8_t value),__LINE__);
INST_PCIW_METHOD(void,uint16_t,set(int x,int y,int c,uint16_t value),__LINE__);
INST_PCIW_METHOD(void,uint32_t,set(int x,int y,int c,uint32_t value),__LINE__);
INST_PCIW_METHOD(void,uint64_t,set(int x,int y,int c,uint64_t value),__LINE__);

//-----------------------------------------------------------------------------
template <class T> void LibImageSegmentation::__PrivateCImgWrapper<T>::resize(double m,int cimgInterpolationType)
{
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  if((int)(m*d.width())!=d.width() || (int)(m*d.height())!=d.height()) 
  {
    d.resize(m*d.width(),m*d.height(),-100,-100,cimgInterpolationType);
    this->width=d.width();
    this->height=d.height();
  }
}
INST_PCIW_METHOD_DEFAULT(void,resize(double m,int cimgInterpolationType));
INST_PCIW_METHOD(void,uint8_t,resize(double m,int cimgInterpolationType),__LINE__);
INST_PCIW_METHOD(void,uint16_t,resize(double m,int cimgInterpolationType),__LINE__);
INST_PCIW_METHOD(void,uint32_t,resize(double m,int cimgInterpolationType),__LINE__);
INST_PCIW_METHOD(void,uint64_t,resize(double m,int cimgInterpolationType),__LINE__);

//-----------------------------------------------------------------------------
template <class T> void LibImageSegmentation::__PrivateCImgWrapper<T>::save(const std::string &filename)const
{
  auto &d=*static_cast<cimg_library::CImg<T> *>(this->cimg);
  d.save(filename.c_str());
}
INST_PCIW_METHOD_DEFAULT(void,save(const std::string &filename)const);
INST_PCIW_METHOD(void,uint8_t,save(const std::string &filename)const,__LINE__);
INST_PCIW_METHOD(void,uint16_t,save(const std::string &filename)const,__LINE__);
INST_PCIW_METHOD(void,uint32_t,save(const std::string &filename)const,__LINE__);
INST_PCIW_METHOD(void,uint64_t,save(const std::string &filename)const,__LINE__);

//-----------------------------------------------------------------------------
int get_index_position(int index,const std::vector<int> *indices)
{
  int ret=-1;
  for(int i=0;i<(int)indices->size();i++) if(indices->at(i)==index) {ret=i;break;}
  return(ret);
}
//------------------------------------------------------------------------------
void _load_mnist_labels(const std::string &labelsFilename,std::vector<int> &resLabels,std::vector<int> *indices)
{
  resLabels.clear();
  if(labelsFilename.size()>0 && (indices==nullptr || indices->size()>0))
  {
    char *buffer=new char[4];
    int i,numofLabels,index;
    std::ifstream in(labelsFilename,std::ifstream::binary);
    if(in.good()) 
    {
      in.read (buffer,4);//magic number
      in.read (buffer,4);//number of images
      numofLabels=(unsigned char)(buffer[3])+
                  256*((unsigned char)(buffer[2]))+
                  256*256*(unsigned char)(buffer[1])+
                  256*256*256*(unsigned char)(buffer[0]);
      
      delete []buffer;
      if(numofLabels>0)
      {
        if(indices==nullptr) resLabels.assign(numofLabels,-0xFFFFFF);
        else resLabels.assign(indices->size(),-0xFFFFFF);

        buffer=new char[numofLabels];

        in.read(buffer,numofLabels);
        if(in.gcount()!=numofLabels) std::cerr << "Warning [_load_mnist_labels]: " << labelsFilename << " is damaged, contains only " << in.gcount() << " labels, " << numofLabels << " expected.\n";
        
        for(i=0;i<in.gcount();i++) 
        {
          if(indices==nullptr) index=i;
          else index=get_index_position(i,indices);
          if(index>=0) resLabels[index]=(int)((unsigned char)buffer[i]);
        }
      }
      else
      {
        throw(LibImageSegmentation::CoreException("Error [_load_mnist_labels]:  " + labelsFilename + ", invalid number of labels: "+std::to_string(numofLabels)+"."));
      }
    }
    else
    {
      throw(LibImageSegmentation::CoreException("Error [_load_mnist_labels]: " + labelsFilename + " does not exist or is not readable."));
    }
    delete []buffer; 
  }
  
}

//-----------------------------------------------------------------------------
template <int N> void _load_mnist_images(const std::string &imagesFilename,
                                  std::vector<LibImageSegmentation::ImageSegmentation<LibImageSegmentation::DefaultTypes::int_type,N> > &resImages,
                                  std::vector<int> *indices)
{
  resImages.clear();
  if(imagesFilename.size()>0 && (indices==nullptr || indices->size()>0))
  {
    std::ifstream in(imagesFilename,std::ifstream::binary);
    int width,height;
    int numofImages=-1;
    char *buffer=new char[4];
    int x,y,a,iid=0;
    int index;
    if(in.good()) 
    {
      in.read(buffer,4);//magic number
      in.read(buffer,4);//number of images
      numofImages=(unsigned char)(buffer[3])+256*((unsigned char)(buffer[2]))+256*256*(unsigned char)(buffer[1])+256*256*256*(unsigned char)(buffer[0]);
      
      in.read(buffer,4);//height (number of rows)
      height=(unsigned char)(buffer[3])+256*((unsigned char)(buffer[2]))+256*256*(unsigned char)(buffer[1])+256*256*256*(unsigned char)(buffer[0]);
      in.read(buffer,4);//width (number of columns)
      width=(unsigned char)(buffer[3])+256*((unsigned char)(buffer[2]))+256*256*(unsigned char)(buffer[1])+256*256*256*(unsigned char)(buffer[0]);
      delete []buffer;
      if(width>0 && height>0)
      {
        if(indices==nullptr) resImages.resize(numofImages);
        else resImages.resize(indices->size());
        buffer=new char[width*height];
        while(in.good())
        {
          in.read(buffer,width*height);
          if(in.gcount()>0)
          {
            if(indices!=nullptr) index=get_index_position(iid,indices);
            if(indices==nullptr || index>=0)
            {
              if(indices==nullptr) index=iid;
              a=0;
              resImages[index].reallocate(width,height);
              for(y=0;y<height;y++)
              {
                for(x=0;x<width;x++)
                {
                  resImages[index].data[x][y]=(unsigned char)(buffer[a]);
                  a++;
                }
              }
              if(in.gcount()!=width*height)
              {
                std::cerr << "Warning [_load_mnist_images]: " << imagesFilename << " is damaged, " << iid << "-th image is incomplete (" << in.gcount() << " bytes read, " << (width*height) << " expected)\n";
                break;
              }
            }
            iid++;
          }
        }
        if(iid!=numofImages) std::cerr << "Warning [_load_mnist_images]: " << imagesFilename << ", number of images in the header and real number of images are different (" << numofImages<<"!=" << iid << ")\n";
      }
      else
      {
        throw(LibImageSegmentation::CoreException("Error [_load_mnist_images]: "+imagesFilename+", invalid width or height (width="+std::to_string(width)+", height"+std::to_string(height)+")."));
      }
    }
    else
    {
      throw(LibImageSegmentation::CoreException("Error [_load_mnist_images]: "+imagesFilename+" does not exist or is not readable."));
    }
    in.close();
   
    delete []buffer; 
  }
}
template void _load_mnist_images(const std::string &imagesFilename,
                                 std::vector<LibImageSegmentation::ImageSegmentation<LibImageSegmentation::DefaultTypes::int_type,LibImageSegmentation::DefaultTypes::SegmentationN> > &resImages,
                                 std::vector<int> *indices);
template void _load_mnist_images(const std::string &imagesFilename,
                                 std::vector<LibImageSegmentation::ImageSegmentation<LibImageSegmentation::DefaultTypes::int_type,LibImageSegmentation::DefaultTypes::SegmentationBlackWhiteN> > &resImages,
                                 std::vector<int> *indices);
template void _load_mnist_images(const std::string &imagesFilename,
                                 std::vector<LibImageSegmentation::ImageSegmentation<LibImageSegmentation::DefaultTypes::int_type,1> > &resImages,
                                 std::vector<int> *indices);

//-----------------------------------------------------------------------------
template <int N> void LibImageSegmentation::load_mnist(const std::string &imagesFilename,
                                                    const std::string &labelsFilename,
                                                    std::vector<ImageSegmentation<DefaultTypes::int_type,N> > &resImages,
                                                    std::vector<int> &resLabels,
                                                    int threshold,
                                                    std::vector<int> *indices)
{
  int x,y;
  _load_mnist_images(imagesFilename,resImages,indices);
  _load_mnist_labels(labelsFilename,resLabels,indices);
  for(auto &&img: resImages)
    for(x=0;x<img.width;x++)
      for(y=0;y<img.height;y++)
        img.data[x][y]=img.data[x][y]>=threshold;
}
template void LibImageSegmentation::load_mnist(const std::string &imagesFilename,
                                            const std::string &labelsFilename,
                                            std::vector<ImageSegmentation<DefaultTypes::int_type,DefaultTypes::SegmentationN> > &resImages,
                                            std::vector<int> &resLabels,
                                            int threshold,
                                            std::vector<int> *indices);
template void LibImageSegmentation::load_mnist(const std::string &imagesFilename,
                                            const std::string &labelsFilename,
                                            std::vector<ImageSegmentation<DefaultTypes::int_type,DefaultTypes::SegmentationBlackWhiteN> > &resImages,
                                            std::vector<int> &resLabels,
                                            int threshold,
                                            std::vector<int> *indices);
//-----------------------------------------------------------------------------
void LibImageSegmentation::load_mnist(const std::string &imagesFilename,
                                   const std::string &labelsFilename,
                                   std::vector<Image<DefaultTypes::int_type> > &resImages,
                                   std::vector<int> &resLabels,
                                   std::vector<int> *indices)
{
  _load_mnist_images(imagesFilename,resImages,indices);
  _load_mnist_labels(labelsFilename,resLabels,indices);
}

