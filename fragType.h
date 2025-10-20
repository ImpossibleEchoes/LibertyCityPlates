#pragma once
#include <Windows.h>

/*
m_pDeathEvents - Вказівник на дані про події смерті?
-
m_fStrength - Величина сили, необхідна для відділення цієї групи від її батьківської групи
m_fForceTransmissionScaleUp - Визначає, яка частина сили, застосованої до цієї групи, передається її батьківській групі(масштабується цим значенням перед передачею)
m_fForceTransmissionScaleDown - Визначає, яка частина сили, застосованої від батьківської групи до цієї групи, застосовується до неї(масштабується цим значенням перед використанням)
m_fJointStiffness - Жорсткість шарніра, до якого прикріплена ця група, якщо він стає шарнірним
m_fMinSoftAngle1 - Мінімальний м'який кутовий ліміт для шарнірів з 1 ступенем свободи (DOF) або перший ліміт для шарнірів з 3 ступенями свободи
m_fMaxSoftAngle1 - Максимальний м'який кутовий ліміт для шарнірів з 1 ступенем свободи (DOF) або перший ліміт для шарнірів з 3 ступенями свободи
m_fMaxSoftAngle2 - Другий максимальний м'який кутовий ліміт для шарнірів з 3 ступенями свободи
m_fMaxSoftAngle3 - Третій максимальний м'який кутовий ліміт для шарнірів з 3 ступенями свободи
m_fRotationSpeed - Швидкість, яку цей шарнір, прикріплений тут, намагатиметься досягти за допомогою м'яза
m_fRotationStrength - Сила, яку цей шарнір використовуватиме, щоб спробувати відповідати швидкості обертання
m_fRestoringStrength - Сила, з якою цей шарнір намагатиметься повернутися до нульової позиції
m_fRestoringMaxTorque - Максимальний крутний момент, який цей шарнір може використовувати для повернення до нульової позиції
m_fLatchStrength - Міцність на розрив засувки на цьому шарнірі, якщо така є
m_fTotalUndamagedMass - Загальна маса всіх дочірніх фрагментів у непошкодженому стані
m_fTotalDamagedMass - Загальна маса всіх дочірніх фрагментів у пошкодженому стані
m_nbChildGroupsPointersIndex - Індекс у fragType до початку списку дочірніх груп, якими володіє ця група(0xFF, якщо їх немає)
m_nbParentGroupPointerIndex - Індекс у fragType до групи, яка володіє цією групою(0xFF, якщо нею володіє корінь)
m_nbChildIndex - Індекс у fragType до дочірнього елемента, яким володіє ця група(0xFF, якщо його немає)
m_nbNumChildren - Кількість дочірніх елементів, якими володіє ця група, починаючи з m_nbChildIndex
m_nbNumChildGroups - Кількість дочірніх груп, якими володіє ця група, починаючи з індексу вказівників на дочірні групи
m_nbGlassModelAndType - це не використовується в IV, тому я взяв назву з RDR1
m_nbGlassPaneModelInfoIndex: (Раніше був індексом геометрії)
m_nbFlags - Використовується лише 0x1 (зникнути після смерті?).0x2 (склом) у rdr1
m_fMinDamadeForce - Мінімальна сила, необхідна для нанесення шкоди цій групі.
m_fDamageHealth - Значення здоров'я, пов'язане з пошкодженням
m_szName - Назва
*/

struct fragTypeGroup {
	void* m_pDeathEvents;				// Pointer to death events data
	uint8_t __4[0xD0 - 0x4];			// Dead event
	float m_fStrength;					// Magnitude of force required to separate this group from its parent
	float m_fForceTransmissionScaleUp;	// Determines how much of a force applied to this group gets passed on to its parent (scaled by this value before transmission)
	float m_fForceTransmissionScaleDown;// Determines how much of a force applied from the parent to this group applies to it (scaled by this value before use)
	float m_fJointStiffness;			// The articulated stiffness of the joint where this group is attached, if it becomes articulated
	float m_fMinSoftAngle1;				// The minimum soft angle limit for 1 DOF (Degrees of Freedom) joints or the first limit for 3 DOF joints
	float m_fMaxSoftAngle1;				// The maximum soft angle limit for 1 DOF joints or the first limit for 3 DOF joints
	float m_fMaxSoftAngle2; 			// The second maximum soft angle limit for 3 DOF joints
	float m_fMaxSoftAngle3; 			// The third maximum soft angle limit for 3 DOF joints
	float m_fRotationSpeed; 			// The speed this articulated joint attached here will attempt to achieve using a muscle
	float m_fRotationStrength;			// The strength this articulated joint will use to try to match the rotation speed
	float m_fRestoringStrength;			// The strength with which this articulated joint will try to reach the zero pose
	float m_fRestoringMaxTorque;		// The maximum torque this articulated joint can use to return to the zero pose
	float m_fLatchStrength;				// The breaking strength of the latch on this joint, if one exists
	float m_fTotalUndamagedMass;		// The total mass of all child fragments when undamaged
	float m_fTotalDamagedMass;			// The total mass of all child fragments when damaged
	uint8_t m_nbChildGroupsPointersIndex;	// Index in the fragType to the start of the list of child groups owned by this group (0xFF if none)
	uint8_t m_nbParentGroupPointerIndex;	// Index in the fragType to the group that owns this group (0xFF if owned by the root)
	uint8_t m_nbChildIndex;				// Index in the fragType to the child owned by this group (0xFF if none)
	uint8_t m_nbNumChildren;				// The number of children owned by this group, starting from m_nbChildIndex
	uint8_t m_nbNumChildGroups;			// The number of child groups owned by this group, starting from child groups pointers index
	uint8_t m_nbGlassModelAndType;			// It's not used in IV, so I took the name from RDR1
	uint8_t m_nbGeometryIndex;				// Who wrote this name if it's not true. I haven't found a single mention that it's true. Rockstar lied in their files. ToDo: check in the gta v sc
	uint8_t m_nbFlags;						// See in db. Used only 0x1(disappear after death). 0x2(become glass) in RDR1
	float m_fMinDamadeForce;			// Minimum force required to cause damage to this group
	float m_fDamageHealth;				// Health value related to damage
	char m_szName[44];					// Name for the group
};