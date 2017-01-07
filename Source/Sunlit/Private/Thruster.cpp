// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Ship.h"
#include "Thruster.h"

void UThruster::SetParticleComponent(UParticleSystemComponent * ParticleComponent)
{
	ThrusterParticles = ParticleComponent;
}

void UThruster::Thrust(float Val, float DeltaTime)
{
	if (ThrusterParticles)
	{
		ThrusterParticles->SetFloatParameter("ThrustAmount", Val*GetEnergyLevel());
		if (Val*GetEnergyLevel() > ThrustThreshold)
			ThrusterParticles->SetFloatParameter("SpawnRate", 1.f);
		else 
			ThrusterParticles->SetFloatParameter("SpawnRate", 0.f);
	}
	if (Val*GetEnergyLevel() > ThrustThreshold) EnergySource->ConsumeBattery(Val*GetEnergyLevel()*ThrustMaxEnergyUse*DeltaTime);
}
