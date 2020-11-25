set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/ECS)

#Library
add_library(
    CloveECS STATIC

    ${INCLUDE}/Component.hpp
	${INCLUDE}/Component.inl
	${INCLUDE}/ComponentManager.hpp
	${INCLUDE}/ComponentManager.inl
	${INCLUDE}/ComponentSet.hpp
	${SOURCE}/ComponentManager.cpp
	${INCLUDE}/ECSEvents.hpp
	${INCLUDE}/ECSTypes.hpp
	${INCLUDE}/Entity.hpp
	${INCLUDE}/Entity.inl
	${SOURCE}/Entity.cpp
	${INCLUDE}/World.hpp
	${INCLUDE}/World.inl
	${SOURCE}/World.cpp
	${INCLUDE}/System.hpp
)

#Includes
target_include_directories(
    CloveECS
    
	PUBLIC
		include

)

#Libraries
target_link_libraries(
    CloveECS

	PUBLIC
		CloveDefinitions
		CloveDeltaTime
		CloveEvent
		CloveMemory
		CloveTimer
)