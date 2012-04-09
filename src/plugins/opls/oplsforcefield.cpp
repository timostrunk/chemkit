/******************************************************************************
**
** Copyright (C) 2009-2011 Kyle Lutz <kyle.r.lutz@gmail.com>
** All rights reserved.
**
** This file is a part of the chemkit project. For more information
** see <http://www.chemkit.org>.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**   * Neither the name of the chemkit project nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************/

// This file implements the OPLS force field. See [Jorgensen 1996].

#include "oplsforcefield.h"

#include <chemkit/plugin.h>
#include <chemkit/foreach.h>
#include <chemkit/topology.h>
#include <chemkit/pluginmanager.h>

#include "oplsatomtyper.h"
#include "oplsparameters.h"
#include "oplscalculation.h"

// --- Construction and Destruction ---------------------------------------- //
OplsForceField::OplsForceField()
    : chemkit::ForceField("opls")
{
    m_parameters = 0;
    setFlags(chemkit::ForceField::AnalyticalGradient);

    const chemkit::Plugin *oplsPlugin = chemkit::PluginManager::instance()->plugin("opls");
    if(oplsPlugin){
        m_parameters = new OplsParameters(oplsPlugin->dataPath() + "oplsaa.prm");
    }
}

OplsForceField::~OplsForceField()
{
    delete m_parameters;
}

// --- Parameterization ---------------------------------------------------- //
bool OplsForceField::setup()
{
    const boost::shared_ptr<chemkit::Topology> &topology = this->topology();
    if(!topology){
        return false;
    }

    foreach(const chemkit::Topology::BondedInteraction &interaction, topology->bondedInteractions()){
        addCalculation(new OplsBondStrechCalculation(interaction[0],
                                                     interaction[1]));
    }

    foreach(const chemkit::Topology::AngleInteraction &interaction, topology->angleInteractions()){
        addCalculation(new OplsAngleBendCalculation(interaction[0],
                                                    interaction[1],
                                                    interaction[2]));
    }

    foreach(const chemkit::Topology::TorsionInteraction &interaction, topology->torsionInteractions()){
        addCalculation(new OplsTorsionCalculation(interaction[0],
                                                  interaction[1],
                                                  interaction[2],
                                                  interaction[3]));
    }

    foreach(const chemkit::Topology::NonbondedInteraction &interaction, topology->nonbondedInteractions()){
        addCalculation(new OplsNonbondedCalculation(interaction[0],
                                                    interaction[1]));
    }

    bool ok = true;

    foreach(chemkit::ForceFieldCalculation *calculation, calculations()){
        bool setup = static_cast<OplsCalculation *>(calculation)->setup(m_parameters);

        if(!setup){
            ok = false;
        }

        setCalculationSetup(calculation, setup);
    }

    return ok;
}
