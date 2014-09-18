#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>


namespace grid {

	using namespace std;
	template<class T>
	class Grid {
	private:
		class DataEntity {
		public:
			unsigned int _id;
			unsigned int _type;
			unsigned int _count;
			T* _data;

			void setData(T*data, int count) {
				_count = count;
				_data = new T[count];
				_type = sizeof(T);
				memcpy(_data, data, count);
			}
			DataEntity(unsigned int id, unsigned int count, T* data) {
				_id = id;
				_count = count;
				_data = new T[count];
				memcpy(_data, data, count*sizeof(T));
			}
			DataEntity() {
				_id = 0;
				_count = 0;
				_data = NULL;

			}
			~DataEntity() {
				if (_data != NULL) {
					if (_count > 1) {
						delete[] _data;
					}
					else {
						delete _data;
					}

				}
			}

		};

		float _x, _y, _z;
		int _width, _height, _depth;
		int _blockByteCount, _blockAtomCount;
		int _order;
		T* _data;
		vector<DataEntity*> _dataEntityList;
		bool _writeWorldCoordsToFile;
		bool _writeBinary;

	public:
		enum{ X_MAJOR_ORDER = 0, Y_MAJOR_ORDER = 1 };
		Grid() {
			_data = 0;
			_width = 1;
			_height = 1;
			_depth = 1;
			_blockAtomCount = 2;
			_x = _y = _z = 0;
			_writeWorldCoordsToFile = false;
			_writeBinary = false;
			_order = Y_MAJOR_ORDER;
		}
		~Grid()  {
			deallocate();
		}
		void setArbitraryWorldXLocation(float x) {
			_writeWorldCoordsToFile = true;
			_x = x;
		}
		void setArbitraryWorldYLocation(float y) {
			_writeWorldCoordsToFile = true;
			_y = y;
		}
		void setArbitraryWorldZLocation(float z) {
			_writeWorldCoordsToFile = true;
			_z = z;
		}
		void addDataEntity(int id, int count, T* data) {
			DataEntity* entity = new DataEntity(id, count, data);
			_dataEntityList.push_back(entity);
		}
		void setOrder(int order) {
			_order = order;
		}
		void setBinaryWriting(bool writeBinary) {
			_writeBinary = writeBinary;
		}

		void setWidth(int i) {
			_width = i;
		}
		void setHeight(int i) {
			_height = i;
		}
		void setDepth(int i) {
			_depth = i;
		}
		void setBlockAtomCount(int i) {
			_blockAtomCount = i;
			_blockByteCount = _blockAtomCount * sizeof(T);
		}
		void setBlockByteCount(int i) {
			_blockByteCount = i;
			_blockAtomCount = _blockByteCount / sizeof(T);
		}

		int getWidth() {
			return _width;
		}
		int getHeight() {
			return _height;
		}
		int getDepth() {
			return _depth;
		}
		int getBlockByteCount() {
			return _blockByteCount;
		}
		int getBlockAtomCount() {
			return _blockAtomCount;
		}

		void allocate() {
			if (_data != 0)deallocate();
			_data = new T[_blockAtomCount*_depth*_height*_width];
			memset(_data, 0, _blockAtomCount*_depth*_height*_width*sizeof(T));
		}
		void deallocate() {

			for (int i = 0; i < _dataEntityList.size(); i++) {

				delete _dataEntityList[i];
			}

			if (_data != 0) {
				delete[] _data;
			}

		}

		void setData(int x, int y, int z, int atom, T data) {
			_data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + atom] = data;
		}
		void setData(int x, int y, int z, T* data) {
			for (int i = 0; i < _blockAtomCount; i++) {
				_data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + i] = data[i];
				cout << x << ",  " << y << " index=" << (((y*_width) + (x))*_blockAtomCount + i) << endl;
			}

		}
		void setBlockType(int x, int y, int z, T type) {
			_data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount] = type;

		}
		void setBlockData(int x, int y, int z, int index, T data) {
			if (index == 0) {
				T old = (T)((int)_data[((y*_width) + (x*_depth) + z)*_blockAtomCount + 1 + index] & 3);
				data = data << 2;
				_data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + 1 + index] = old | data;
			}
			else {
				_data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + 1 + index] = data;
			}

		}
		void setBlockOrientation(int x, int y, int z, int orientation) {
			T old = (T)((int)_data[((y*_width) + (x*_depth) + z)*_blockAtomCount + 1] & 252);
			_data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + 1] = (old | orientation);
		}
		T getData(int x, int y, int z, int atom) {
			return _data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + atom];
		}
		T* getData(int x, int y, int z) {
			return _data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount];
		}
		bool getData_safe(int x, int y, int z, T* buffer) {

			for (int i; i < _blockAtomCount; i++) {
				buffer[i] = _data[((y*_width*_depth) + (x*_depth) + z)*_blockAtomCount + i];
			}
		}

		void setAllBlockType(T type) {
			for (int x = 0; x < this->_width; x++) {
				for (int y = 0; y < this->_height; y++) {
					for (int z = 0; z < this->_depth; z++) {
						setBlockType(x, y, z, type);
					}
				}
			}
		}
		void setAllBlockData(T data, int index) {
			for (int x = 0; x < this->_width; x++) {
				for (int y = 0; y < this->_height; y++) {
					for (int z = 0; z < this->_depth; z++) {
						setBlockData(x, y, z, index, data);
					}
				}
			}
		}
		bool load(string filename, bool binary) {
			if (binary) {
				ifstream stream;
				stream.open(filename, ios::in | ios::binary);


				if (!stream.is_open())return false;

				return true;
			}
			else {

			}
		}
		bool save(string filename) {
			if (_writeBinary) {
				ofstream stream;
				int atomSize = sizeof(T);
				stream.open(filename, ios::out | ios::binary);
				if (!stream.is_open())return false;

				stream << "ggf";
				stream.write((const char*)&_width, sizeof(int));
				stream.write((const char*)&_height, sizeof(int));
				stream.write((const char*)&_depth, sizeof(int));
				stream.write((const char*)&_x, sizeof(float));
				stream.write((const char*)&_y, sizeof(float));
				stream.write((const char*)&_z, sizeof(float));
				stream.write((const char*)&atomSize, sizeof(int));
				stream.write((const char*)&_blockAtomCount, sizeof(int));
				stream.write((const char*)&_order, sizeof(int));

				stream << "@cnk";
				for (int x = 0; x < this->_width; x++) {
					for (int y = 0; y < this->_height; y++) {
						for (int z = 0; z < this->_depth; z++) {
							for (int a = 0; a < this->_blockAtomCount; a++) {
								T data = (_order == X_MAJOR_ORDER) ? getData(x, y, z, a) : getData(y, x, z, a);

								stream.write((const char*)&data, sizeof(T));

							}

						}
					}

				}

				for (int i = 0; i < _dataEntityList.size(); i++) {

					stream << "@dat";
					cout << sizeof(T) << endl;
					cout << _dataEntityList[i]->_count << endl;
					cout << sizeof(int)* 4 + (sizeof(T)*(_dataEntityList[i]->_count)) << endl;
					//stream.write((const char*)(_dataEntityList[i]), sizeof(int)* 4 + (sizeof(T)*(_dataEntityList[i]->_count)));
					stream.write((const char*)&(_dataEntityList[i]->_id), sizeof(int));
					stream.write((const char*)&(_dataEntityList[i]->_type), sizeof(int));
					stream.write((const char*)&(_dataEntityList[i]->_count), sizeof(int));
					stream.write((const char*)(_dataEntityList[i]->_data), sizeof(T)*(_dataEntityList[i]->_count));
				}

				stream.close();
			}
			else {
				ofstream stream;
				stream.open(filename, ios::out);
				if (!stream.is_open())return false;

				stream << "chunk_size: " << _width << " " << _height << " " << _depth << std::endl;
				stream << "atom_size: " << sizeof(T) << std::endl;
				stream << "block_atom_count: " << _blockAtomCount << std::endl;
				if (_writeWorldCoordsToFile) {
					stream << "world_coords: " << _x << " " << _y << " " << _z << std::endl;
				}
				stream << "order: " << _order << std::endl;
				int atomSize = sizeof(T);
				char* res = new char[atomSize * 2 + 1];

				for (int x = 0; x < this->_width; x++) {
					for (int y = 0; y < this->_height; y++) {
						for (int z = 0; z < this->_depth; z++) {
							for (int a = 0; a < this->_blockAtomCount; a++) {
								T data = (_order == X_MAJOR_ORDER) ? getData(x, y, z, a) : getData(y, x, z, a);

								int atomMaxSize = pow(256, atomSize);
								cout << atomMaxSize << endl;
								if (data <= atomMaxSize)
								{
									if (atomSize < 2) {
										sprintf_s(res, atomSize * 2 + 1, "%02x", data);
									}
									else if (atomSize < 4) {
										sprintf_s(res, atomSize * 2 + 1, "%04x", data);
									}
									else {
										sprintf_s(res, atomSize * 2 + 1, "%08x", data);
									}

									stream << res;
								}

								//cout << x << ", " << y << ", " << z << ", " << a << " h=" << res << endl;
							}
							stream << " ";
						}
					}
					stream << "\n";
				}
				delete[] res;
				stream.close();
			}
			return true;
		}




	};
	typedef Grid<unsigned char> ByteGrid;
	typedef Grid<unsigned short> ShortGrid;
	typedef Grid<unsigned int> IntGrid;
	typedef Grid<unsigned long long> LongGrid;

	typedef ShortGrid StandardGrid;
}

#endif

