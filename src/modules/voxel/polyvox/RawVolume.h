#pragma once

#include "BaseVolume.h"
#include "Voxel.h"
#include <cstdlib> //For abort()
#include <limits>
#include <memory>

namespace voxel {

/**
 * Simple volume implementation which stores data in a single large 3D array.
 *
 * This class is less memory-efficient than the PagedVolume, but it is the simplest possible
 * volume implementation which makes it useful for debugging and getting started with PolyVox.
 */
class RawVolume: public BaseVolume<Voxel> {
public:
	//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
	//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
	//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
	//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
	//in the future
	//typedef Volume<Voxel> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences.
	//class Sampler : public VolumeOfVoxelType::template Sampler< RawVolume >
#if defined(_MSC_VER)
	class Sampler : public BaseVolume<Voxel>::Sampler< RawVolume> //This line works on VS2010
#else
	class Sampler: public BaseVolume<Voxel>::template Sampler<RawVolume> //This line works on GCC
#endif
	{
	public:
		Sampler(RawVolume* volume);
		~Sampler();

		inline const Voxel& getVoxel() const;

		inline bool isCurrentPositionValid() const;

		inline void setPosition(const glm::ivec3& v3dNewPos);
		void setPosition(int32_t xPos, int32_t yPos, int32_t zPos);
		inline bool setVoxel(const Voxel& tValue);

		void movePositiveX();
		void movePositiveY();
		void movePositiveZ();

		void moveNegativeX();
		void moveNegativeY();
		void moveNegativeZ();

		inline Voxel peekVoxel1nx1ny1nz() const;
		inline Voxel peekVoxel1nx1ny0pz() const;
		inline Voxel peekVoxel1nx1ny1pz() const;
		inline Voxel peekVoxel1nx0py1nz() const;
		inline Voxel peekVoxel1nx0py0pz() const;
		inline Voxel peekVoxel1nx0py1pz() const;
		inline Voxel peekVoxel1nx1py1nz() const;
		inline Voxel peekVoxel1nx1py0pz() const;
		inline Voxel peekVoxel1nx1py1pz() const;

		inline Voxel peekVoxel0px1ny1nz() const;
		inline Voxel peekVoxel0px1ny0pz() const;
		inline Voxel peekVoxel0px1ny1pz() const;
		inline Voxel peekVoxel0px0py1nz() const;
		inline Voxel peekVoxel0px0py0pz() const;
		inline Voxel peekVoxel0px0py1pz() const;
		inline Voxel peekVoxel0px1py1nz() const;
		inline Voxel peekVoxel0px1py0pz() const;
		inline Voxel peekVoxel0px1py1pz() const;

		inline Voxel peekVoxel1px1ny1nz() const;
		inline Voxel peekVoxel1px1ny0pz() const;
		inline Voxel peekVoxel1px1ny1pz() const;
		inline Voxel peekVoxel1px0py1nz() const;
		inline Voxel peekVoxel1px0py0pz() const;
		inline Voxel peekVoxel1px0py1pz() const;
		inline Voxel peekVoxel1px1py1nz() const;
		inline Voxel peekVoxel1px1py0pz() const;
		inline Voxel peekVoxel1px1py1pz() const;

	private:

		//Other current position information
		Voxel* mCurrentVoxel;

		//Whether the current position is inside the volume
		//FIXME - Replace these with flags
		bool m_bIsCurrentPositionValidInX;
		bool m_bIsCurrentPositionValidInY;
		bool m_bIsCurrentPositionValidInZ;
	};

public:
	/// Constructor for creating a fixed size volume.
	RawVolume(const Region& regValid);

	/// Destructor
	~RawVolume();

	/// Gets the value used for voxels which are outside the volume
	Voxel getBorderValue() const;
	/// Gets a Region representing the extents of the Volume.
	const Region& getEnclosingRegion() const;

	/// Gets the width of the volume in voxels.
	int32_t getWidth() const;
	/// Gets the height of the volume in voxels.
	int32_t getHeight() const;
	/// Gets the depth of the volume in voxels.
	int32_t getDepth() const;

	/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
	const Voxel& getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const;
	/// Gets a voxel at the position given by a 3D vector
	inline const Voxel& getVoxel(const glm::ivec3& v3dPos) const;

	/// Sets the value used for voxels which are outside the volume
	void setBorderValue(const Voxel& tBorder);
	/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
	void setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, Voxel tValue);
	/// Sets the voxel at the position given by a 3D vector
	inline void setVoxel(const glm::ivec3& v3dPos, Voxel tValue);

	/// Calculates approximatly how many bytes of memory the volume is currently using.
	uint32_t calculateSizeInBytes();

protected:
	/// Copy constructor
	RawVolume(const RawVolume& rhs);

	/// Assignment operator
	RawVolume& operator=(const RawVolume& rhs);

private:
	void initialise(const Region& regValidRegion);

	//The size of the volume
	Region m_regValidRegion;

	//The border value
	Voxel m_tBorderValue;

	//The voxel data
	Voxel* m_pData;
};

////////////////////////////////////////////////////////////////////////////////
/// This version of the function is provided so that the wrap mode does not need
/// to be specified as a template parameter, as it may be confusing to some users.
/// @param v3dPos The 3D position of the voxel
/// @return The voxel value
////////////////////////////////////////////////////////////////////////////////
inline const Voxel& RawVolume::getVoxel(const glm::ivec3& v3dPos) const {
	return getVoxel(v3dPos.x, v3dPos.y, v3dPos.z);
}

////////////////////////////////////////////////////////////////////////////////
/// @param v3dPos the 3D position of the voxel
/// @param tValue the value to which the voxel will be set
////////////////////////////////////////////////////////////////////////////////
inline void RawVolume::setVoxel(const glm::ivec3& v3dPos, Voxel tValue) {
	setVoxel(v3dPos.x, v3dPos.y, v3dPos.z, tValue);
}

#define CAN_GO_NEG_X(val) (val > this->mVolume->getEnclosingRegion().getLowerX())
#define CAN_GO_POS_X(val) (val < this->mVolume->getEnclosingRegion().getUpperX())
#define CAN_GO_NEG_Y(val) (val > this->mVolume->getEnclosingRegion().getLowerY())
#define CAN_GO_POS_Y(val) (val < this->mVolume->getEnclosingRegion().getUpperY())
#define CAN_GO_NEG_Z(val) (val > this->mVolume->getEnclosingRegion().getLowerZ())
#define CAN_GO_POS_Z(val) (val < this->mVolume->getEnclosingRegion().getUpperZ())

inline const Voxel& RawVolume::Sampler::getVoxel() const {
	if (this->isCurrentPositionValid()) {
		return *mCurrentVoxel;
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
}

inline bool RawVolume::Sampler::isCurrentPositionValid() const {
	return m_bIsCurrentPositionValidInX && m_bIsCurrentPositionValidInY && m_bIsCurrentPositionValidInZ;
}

inline void RawVolume::Sampler::setPosition(const glm::ivec3& v3dNewPos) {
	setPosition(v3dNewPos.x, v3dNewPos.y, v3dNewPos.z);
}

inline bool RawVolume::Sampler::setVoxel(const Voxel& tValue) {
	//return m_bIsCurrentPositionValid ? *mCurrentVoxel : this->mVolume->getBorderValue();
	if (this->m_bIsCurrentPositionValidInX && this->m_bIsCurrentPositionValidInY && this->m_bIsCurrentPositionValidInZ) {
		*mCurrentVoxel = tValue;
		return true;
	}
	return false;
}

Voxel RawVolume::Sampler::peekVoxel1nx1ny1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel1nx1ny0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume)) {
		return *(mCurrentVoxel - 1 - this->mVolume->getWidth());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel1nx1ny1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel1nx0py1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel1nx0py0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume)) {
		return *(mCurrentVoxel - 1);
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel1nx0py1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel1nx1py1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel1nx1py0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume)) {
		return *(mCurrentVoxel - 1 + this->mVolume->getWidth());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel1nx1py1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel0px1ny1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel0px1ny0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_Y(this->mYPosInVolume)) {
		return *(mCurrentVoxel - this->mVolume->getWidth());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel0px1ny1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel0px0py1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel0px0py0pz() const {
	if (this->isCurrentPositionValid()) {
		return *mCurrentVoxel;
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel0px0py1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel0px1py1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel0px1py0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_Y(this->mYPosInVolume)) {
		return *(mCurrentVoxel + this->mVolume->getWidth());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel0px1py1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel1px1ny1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel1px1ny0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume)) {
		return *(mCurrentVoxel + 1 - this->mVolume->getWidth());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel1px1ny1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel1px0py1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel1px0py0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume)) {
		return *(mCurrentVoxel + 1);
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel1px0py1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume + 1);
}

Voxel RawVolume::Sampler::peekVoxel1px1py1nz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
}

Voxel RawVolume::Sampler::peekVoxel1px1py0pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume)) {
		return *(mCurrentVoxel + 1 + this->mVolume->getWidth());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume);
}

Voxel RawVolume::Sampler::peekVoxel1px1py1pz() const {
	if (this->isCurrentPositionValid() && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume)) {
		return *(mCurrentVoxel + 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
	}
	return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
}

#undef CAN_GO_NEG_X
#undef CAN_GO_POS_X
#undef CAN_GO_NEG_Y
#undef CAN_GO_POS_Y
#undef CAN_GO_NEG_Z
#undef CAN_GO_POS_Z

}
