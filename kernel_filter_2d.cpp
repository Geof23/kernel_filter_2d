#include <sail-c++/sail-c++.h>
#include <iostream>
#include <string>
#include <span>
#include <vector>
#include <cstdint>
#include <cstring>
#include <new>
#include <memory>

using namespace std;

using intp_t = int32_t;

template <typename T>
struct pixela {
  T r;
  T g;
  T b;
  T a;
  pixela():
    r(0),g(0),b(0),a(0){}
  pixela(T r, T g, T b, T a):
      r(r),g(g),b(b),a(a){}
  template <typename R>
  void
  operator*=(const R s){
    r *= s;
    g *= s;
    b *= s;
    a *= s;
  }
  template <typename R>
  pixela<intp_t>
  operator*(const R s) const{
    return pixela<intp_t>(r*s, g*s, b*s, a*s);
  }
  template <typename R>
  void
  operator+=(pixela<R> const& rhs){
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
  }
};

using spix_t = pixela<uint8_t>;
using intpix_t = pixela<intp_t>;

template <typename T>
struct kernel;

//rows & cols must be equal for lhs & rhs
//image convolution operator
template <typename M, typename K>
intpix_t
operator*(const M& lhs, const kernel<K>& rhs){
  intpix_t rv;
  for(int y = 0; y < rhs.w; ++y)
    for(int x = 0; x < rhs.w; ++x){
      rv += lhs(x,y) * rhs(x,y);
    }
  rv *= rhs.fact;
  return rv;
}

template <typename T>
struct kernel {
  vector<T> m;
  size_t w;
  float fact;
  string label;
  typedef T value_type;
  const T&
  operator()(int x, int y) const {
    return m[x+y*w];
  }
};

template <typename C>
struct view {
  view(size_t stride, C&& array):
    stride(stride), array(move(array)){}
  size_t stride;
  C array;
  const typename C::value_type&
  operator()(int x, int y) const{
    return array[x + (y*stride)];
  }
};

class filter {
  string fext;
  string inpath;
  string outpath;
  sail::image image;
  sail::codec_info codec_info;
  sail::write_options write_options;
  unique_ptr<void, void(*)(void*)> srcPixels;
  kernel<intp_t> k;
  vector<intpix_t> dpixels;
  vector<spix_t> opixels;

  void
  squash(){ //clamping the output to r8g8b8a8
    auto sp = dpixels.begin();
    auto op = opixels.begin();
    for(; sp < dpixels.end(); ++sp,++op){
      op->r = min(255, max(0, sp->r));
      op->g = min(255, max(0, sp->g));
      op->b = min(255, max(0, sp->b));
      //HACK png uses alpha -- some filters end up setting to 0 (like edge filters)
      op->a = 255; //min(255, max(0, sp->a));
    }
  }
public:
  filter(const string inpath):
    fext{inpath.substr(inpath.find_last_of('.')+1, string::npos)},
    inpath{inpath}, outpath{},
    image{}, codec_info{}, write_options{},
    srcPixels{nullptr,::operator delete}, k{}, dpixels{}, opixels{}
  {
    sail::image_reader reader;
    sail::codec_info::from_extension(fext, &codec_info);
    sail::read_options read_options;
    codec_info.read_features().to_read_options(&read_options);
    read_options = read_options.with_output_pixel_format(SAIL_PIXEL_FORMAT_BPP32_RGBA);
    reader.start_reading(inpath, codec_info, read_options);
    reader.read_next_frame(&image);
    reader.stop_reading();
    
    dpixels.resize(image.height()*image.width());
    opixels.resize(image.height()*image.width());

    codec_info.write_features().to_write_options(&write_options);
    size_t psize = 4 * image.height() * image.width();
    srcPixels.reset(::operator new(psize));
    memcpy(srcPixels.get(), image.pixels(), psize);
    
    cout << "loaded " << inpath << ", " << image.width() << "x" << image.height() << ", " << fext << '\n';
  }
  
  filter&
  process(const kernel<intp_t> kin){
    k = kin;
    outpath = {inpath.
	    substr(0,inpath.find_last_of('.')) +
	    "_" +
	    k.label + "." +
	    fext
    };

    cout << "applying " << k.label << '\n';
    
    int skip = k.w / 2; //skips edges
    for(auto y = skip; y < image.height()-skip; ++y){
      for(auto x = skip; x < image.width()-skip; ++x){
	const auto srcBegin = (x-skip) + (y-skip)*image.width();
	auto& dp = dpixels[x + y * image.width()];
	dp =
	  view(image.width(),
	       span<spix_t>{
		 static_cast<spix_t*>(srcPixels.get()) +
		   srcBegin,
		   image.width() * image.height() - srcBegin})
	  * k;
      }
    }
    return *this;
  }

  void
  write(){
    squash();
    sail::image_writer writer;
    writer.start_writing(outpath, codec_info, write_options);

    writer.write_next_frame(image.with_shallow_pixels(opixels.data()));

    writer.stop_writing();
  }
};

int
main(int argc, char* argv[]){
  sail_set_log_barrier(SAIL_LOG_LEVEL_SILENCE);
  sail_init_with_flags(SAIL_FLAG_PRELOAD_CODECS);
  //kernels: {{kernel matrix}, matrix order, global coefficient, name}
  vector<kernel<intp_t>> kernels =
    {
     {{0, 0, 0,
       0, 1, 0,
       0, 0, 0},
      3, 1.f,
      "Identity"},
     {{1, 0,-1,
       0, 0, 0,
       -1, 0, 1},
      3, 1.f,
      "EdgeD"},
     {{0,-1, 0,
       -1, 4, -1,
       0, -1, 0},
      3, 1.f,
      "EdgeL"},
     {{-1,-1, -1,
       -1, 8, -1,
       -1, -1, -1},
      3, 1.f,
      "EdgeC"},
     {{0,-1, 0,
       -1, 5, -1,
       0, -1, 0},
      3, 1.f,
      "Sharpen"},
     {{1, 1, 1,
       1, 1, 1,
       1, 1, 1},
      3, 1.f/9.f,
      "BoxBlur"},
     {{1, 2, 1,
       2, 4, 2,
       1, 2, 1},
      3, 1.f/16.f,
      "GaussianBlur_3x3"},
     {{1,4,6,4,1,
       4,16,24,16,4,
       6,24,36,24,6,
       4,16,24,16,4,
       1,4,6,4,1},
      5, 1.f/256.f,
      "GaussianBlur_5x5"},
     {{1,4,6,4,1,
       4,16,24,16,4,
       6,24,-476,24,6,
       4,16,24,16,4,
       1,4,6,4,1},
      5, -1.f/256.f,
      "UnsharpMasking_5x5"}
    };
  filter f(argv[1]);
  for(auto& k: kernels) f.process(k).write();
}
