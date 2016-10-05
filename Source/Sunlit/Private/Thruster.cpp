// Fill out your copyright notice in the Description page of Project Settings.

#include "Sunlit.h"
#include "Thruster.h"

void UThruster::SetParticleComponent(UParticleSystemComponent * ParticleComponent)
{
	ThrusterParticles = ParticleComponent;
}

void UThruster::Thrust(float Val)
{
	if (ThrusterParticles)
	{
		ThrusterParticles->SetFloatParameter("ThrustAmount", Val*GetEnergyLevel());
		if (Val*GetEnergyLevel() > 0.1f)
			ThrusterParticles->SetFloatParameter("SpawnRate", 1.f);
		else 
			ThrusterParticles->SetFloatParameter("SpawnRate", 0.f);
	}
}
