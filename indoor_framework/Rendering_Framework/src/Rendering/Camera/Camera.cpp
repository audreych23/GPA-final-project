#include "Camera.h"

#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>

#include <iostream>

namespace INANOA {

	Camera::Camera(const glm::vec3& viewOrg, const glm::vec3& lookCenter, const glm::vec3& upVector, const float distance, const float fovDeg, const float fNear, const float fFar) : MIN_DISTANCE(0.05f){
		this->m_viewportWidth = 64;
		this->m_viewportHeight = 64;
		this->m_fovDeg = fovDeg;
		this->m_near = fNear;
		this->m_far = fFar;
		this->resize(64, 64);
		this->reset(viewOrg, lookCenter, upVector, distance);	
				
	}
	Camera::Camera() : MIN_DISTANCE(0.1f) {
		this->m_viewportWidth = 64;
		this->m_viewportHeight = 64;
		this->m_fovDeg = 45.0f;
		this->m_near = 1.0f;
		this->m_far = 64.0f;
		this->resize(64, 64);
		this->reset(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -1.0f);	
	}

	void Camera::reset(const glm::vec3& viewOrg, const glm::vec3& lookCenter, const glm::vec3& upVector, const float distance) {
		this->m_distance = distance;
		if (distance < 0.0f) {
			this->m_distance = glm::length(viewOrg - lookCenter);
		}

		this->setViewOrg(viewOrg);
		this->setLookCenter(lookCenter);
		this->setUpVector(upVector);

		this->update();
	}

	Camera::~Camera(){}

	void Camera::resize(const int w, const int h) {
		this->m_viewportWidth = w;
		this->m_viewportHeight = h;
		// update projection matrix
		this->m_projMat = glm::perspective(glm::radians(this->m_fovDeg), w * 1.0f / h, this->m_near, this->m_far);
		// inverse projection matrix
		this->m_inverseProjMat = glm::inverse(this->m_projMat);
	}
	
	void Camera::update() {
		glm::vec3 centerToEyeVec = glm::normalize(this->m_viewOrg - this->m_lookCenter);
		this->setViewOrigBasedOnLookCenter(this->m_distance * centerToEyeVec);

		this->m_viewMat = glm::lookAt(this->m_viewOrg, this->m_lookCenter, this->m_upVector);
		
		// rotation part
		glm::mat4 rMat = this->m_viewMat;
		rMat[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		rMat = glm::transpose(rMat);
		// translate part
		glm::mat4 tMat = glm::translate(this->m_viewOrg);
		// model matrix (inverse view matrix)
		glm::mat4 invView = tMat * rMat;
		this->m_modelMat = invView;
		// calculate View-Projection inverse matrix
		this->m_inverseVPMat = invView * this->m_inverseProjMat;
	}

	void Camera::setViewOrigBasedOnLookCenter(const glm::vec3& tVec) {
		this->m_viewOrg = this->m_lookCenter + tVec;
	}
	void Camera::translateLookCenterAndViewOrg(const glm::vec3& t) {
		this->m_lookCenter = this->m_lookCenter + t;
		this->m_viewOrg = this->m_viewOrg + t;
	}
	void Camera::forward(const glm::vec3& forwardMagnitude, const bool disableYDimension) {
		glm::mat4 vmt = glm::transpose(this->viewMatrix());
		glm::vec4 forward = vmt * glm::vec4(forwardMagnitude, 0);

		if (disableYDimension) {
			forward.y = 0.0;
		}

		this->translateLookCenterAndViewOrg(forward);
	}
	void Camera::rotateLookCenterAccordingToViewOrg(const float rad) {
		this->setLookCenter(
			Camera::rotateLookCenterAccordingToViewOrg(this->m_lookCenter, this->m_viewOrg, this->m_viewMat, rad)
		);
	}

	glm::vec3 Camera::rotateLookCenterAccordingToViewOrg(const glm::vec3& center, const glm::vec3& eye, const glm::mat4& viewMat, const float rad) {
		glm::mat4 vmt = glm::transpose(viewMat);
		glm::vec3 yAxis(vmt[1].x, vmt[1].y, vmt[1].z);
		glm::quat q = glm::angleAxis(rad, yAxis);
		glm::mat4 rotMat = glm::toMat4(q);

		glm::vec3 p = center - eye;
		glm::vec4 resP = rotMat * glm::vec4(p.x, p.y, p.z, 1.0);

		return glm::vec3(resP.x + eye.x, resP.y + eye.y, resP.z + eye.z);
	}

	void Camera::viewFrustumClipPlaneCornersInViewSpace(const float depth, float* corners) const {
		// Get Depth in NDC, the depth in viewSpace is negative
		const glm::vec4 v = glm::vec4(0, 0, -1 * depth, 1);
		glm::vec4 vInNDC = this->m_projMat * v;		
		vInNDC.z = vInNDC.z / vInNDC.w;

		// Get 4 corner of the clip plane
		float cornerXY[] = {
			-1, 1,
			-1, -1,
			1, -1,
			1, 1
		};
		for (int j = 0; j < 4; j++) {
			glm::vec4 wcc = {
				cornerXY[j * 2 + 0], cornerXY[j * 2 + 1], vInNDC.z, 1
			};
			wcc = this->m_inverseProjMat * wcc;
			wcc = wcc / wcc.w;

			corners[j * 3 + 0] = wcc[0];
			corners[j * 3 + 1] = wcc[1];
			corners[j * 3 + 2] = wcc[2];
		}
	}

	//void Camera::rotateUpDownLeftRight(float x_offset, float y_offset, bool constraint_pitch) {
	//	// Mouse sensitivity (you can adjust this)
	//	const float sensitivity = 0.0001f;

	//	x_offset *= sensitivity;
	//	y_offset *= sensitivity;

	//	// Yaw rotation (left/right)
	//	glm::vec3 forwardVec = glm::normalize(this->m_lookCenter - this->m_viewOrg);
	//	glm::quat yawQuat = glm::angleAxis(glm::radians(x_offset), this->m_upVector);

	//	// Pitch rotation (up/down)
	//	glm::vec3 rightAxis = glm::normalize(glm::cross(forwardVec, this->m_upVector));
	//	float currentPitch = std::asin(glm::clamp(forwardVec.y, -1.0f, 1.0f));
	//	float newPitch = currentPitch - y_offset; // Inverted to match typical mouse look

	//	// Optional pitch clamping
	//	if (constraint_pitch) {
	//		newPitch = glm::clamp(newPitch, glm::radians(-89.0f), glm::radians(89.0f));
	//	}

	//	float pitchRad = newPitch - currentPitch;
	//	glm::quat pitchQuat = glm::angleAxis(pitchRad, rightAxis);

	//	// Combine rotations
	//	glm::vec3 newViewOrg = this->m_viewOrg - this->m_lookCenter; // Translate to origin
	//	newViewOrg = yawQuat * newViewOrg;  // Yaw rotation
	//	newViewOrg = pitchQuat * newViewOrg;  // Pitch rotation

	//	this->m_viewOrg = this->m_lookCenter + newViewOrg;  // Translate back
	//}

	void Camera::rotateUpDown(const float rad, bool constraint_pitch) {
		// Calculate the forward vector (view direction)
		glm::vec3 forwardVec = glm::normalize(this->m_lookCenter - this->m_viewOrg);
		// Calculate the right axis (perpendicular to forward and up)
		glm::vec3 rightAxis = glm::normalize(glm::cross(forwardVec, this->m_upVector));

		// Pitch clamp (typically between -89 and 89 degrees)
		float currentPitch = std::asin(glm::clamp(forwardVec.y, -1.0f, 1.0f));
		float newPitch = currentPitch + (rad * 0.5f);

		// Clamp the pitch to prevent camera from flipping
		if (constraint_pitch)
			newPitch = glm::clamp(newPitch, glm::radians(-89.0f), glm::radians(89.0f));
		float clampedRad = newPitch - currentPitch;

		// Create a quaternion for the rotation
		glm::quat q = glm::angleAxis(clampedRad, rightAxis);

		// Apply the rotation to the view origin
		glm::vec3 newViewOrg = this->m_viewOrg - this->m_lookCenter; // Translate to origin
		newViewOrg = q * newViewOrg;                                // Rotate
		this->m_viewOrg = this->m_lookCenter + newViewOrg;          // Translate back

		// Update the camera's matrices
		//this->update();
	}

	void Camera::rotateLeftRight(const float rad) {
		// Calculate the forward vector (view direction)
		glm::vec3 forwardVec = glm::normalize(this->m_lookCenter - this->m_viewOrg);

		// Create a quaternion for the rotation around the up vector
		glm::quat q = glm::angleAxis(rad * 0.5f, this->m_upVector);

		// Apply the rotation to the view origin
		glm::vec3 newViewOrg = this->m_viewOrg - this->m_lookCenter; // Translate to origin
		newViewOrg = q * newViewOrg;                                // Rotate
		this->m_viewOrg = this->m_lookCenter + newViewOrg;          // Translate back
	}

}


