/* 
 * File:   Setowq.h
 * Author: shlomi
 *
 * Created on 30 דצמבר 2015, 08:44
 */

#ifndef SETOWQ_H
#define	SETOWQ_H

#ifndef OWQ_DEBUG
	 /**
	 * Set Defaul debuger output
	 */
    #define OWQ_DEBUG false
#endif

#ifndef OWQ_DEBUG_LEVEL
    /**
     * 
     */
    #define OWQ_DEBUG_LEVEL 4
#endif

#ifndef OWQ_DEBUG_EXPOSE_COMPILER_PARSE
    /**
     * Require debug level 1
     */
    #define OWQ_DEBUG_EXPOSE_COMPILER_PARSE true
#endif

#ifndef OWQ_DEBUG_EXPOSE_COMPILER_MACRO_USE
    /**
     * Require debug level 1
     */
    #define OWQ_DEBUG_EXPOSE_COMPILER_MACRO_USE true
#endif

#ifndef OWQ_DEBUG_EXPOSE_FUNCTION_TABLE
    /**
     * Require debug level 2
     */
    #define OWQ_DEBUG_EXPOSE_FUNCTION_TABLE true
#endif

#ifndef OWQ_DEBUG_EXPOSE_COMPILED_CODE
    /**
     * Require debug level 1
     */
    #define OWQ_DEBUG_EXPOSE_COMPILED_CODE true
#endif

#ifndef OWQ_DEBUG_EXPOSE_EXECUTION_STEPS
    /**
     * Require debug level 2
     */
    #define OWQ_DEBUG_EXPOSE_EXECUTION_STEPS true
#endif

#ifndef OWQ_DEBUG_EXPOSE_EXECUTIOM_STACK_STATE
    /**
     * Require debug level 3
     * And Require OWQ_DEBUG_EXPOSE_EXECUTION_STEPS
     */
    #define OWQ_DEBUG_EXPOSE_EXECUTIOM_STACK_STATE true
#endif

#endif	/* SETOWQ_H */

