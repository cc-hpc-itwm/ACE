/** \file
 * \author Leo Nesemann, ITWM
 * \date 2013-03
 * \copyright Fraunhofer ITWM
 */

/** \namespace ace
 * \brief local scheduling class and algorithms
 *
 * This namespace provides a naive scheduling algorithm for local parallelisation. It is assumed that
 * single tasks are repeatedly executed for a sequence of increasing or decreasing time steps, or that
 * they are executed exactly once.
 *
 * The class scheduler::Schedule will manage the single work packages. Work packages inherit from the
 * abstract class scheduler::Executable. Examples for such work packages are elastic::iso::Compute, which
 * applies a finite difference kernel to a subvolume to compute the next step, or elastic::Unpack, which
 * unpacks a halo buffer that has been sent from another node.
 * \n
 * The schedule consists of an executable and a condition, which must be inherited from the abstract
 * class scheduler::Condition. Only if the condition is met (i.e., the implementation of
 * scheduler::Condition::check() returns \b true ), the work package is run by the implementation of
 * scheduler::Executable::execute(). An example condition is elastic::StencilHasOldTimestamp, which
 * checks whether the surrounding subvolumes have a given timestamp: Only if this is true, the stencil
 * can be applied.
 * \n
 * It is possible to run several work packages subsequently, as soon as a condition is satisfied. For
 * this, the class scheduler::ExecutableList can be used. For example, this is done for the simultaneous
 * backward propagation and imaging in the elastic case.
 *
 * As soon as the schedule for the current job (initialisation, forward propagation, backward
 * propagation etc.) is filled, it must be handed to an instance of scheduler::Threading. Calling
 * scheduler::Threading::beginComputation() and scheduler::Threading::endComputation() will then execute
 * the work packages in parallel.
 * \n
 * A typical workflow is:
 *  - Get an instance of scheduler::Threading.
 *  - Create a new schedule and fill it with initialisation tasks.
 *  - Call scheduler::Threading::setSchedule() with the initialisation schedule.
 *  - Call scheduler::Threading::beginComputation() and scheduler::Threading::endComputation().\n
 *    After this, the initialisation is finished.
 *  - Create a new schedule for the forward propagation, insert packing, computing and unpacking tasks.
 *  - Call scheduler::Threading::setSchedule() with this forward propagation schedule.
 *  - Run the forward propagation, again with calls to scheduler::Threading::beginComputation()
 *    and scheduler::Threading::endComputation().
 *  - Create another schedule for the backward propagation, including packing and unpacking tasks for
 *    both the source and receiver field, as well as computation and imaging work packages.
 *  - Set this schedule and execute again as before.
 */
