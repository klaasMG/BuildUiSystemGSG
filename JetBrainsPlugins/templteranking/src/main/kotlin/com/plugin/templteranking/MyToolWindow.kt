package com.plugin.templateranker

import com.intellij.codeInsight.completion.*
import com.intellij.codeInsight.lookup.LookupElement
import com.intellij.codeInsight.lookup.LookupElementDecorator
import com.intellij.codeInsight.lookup.LookupElementWeigher
import com.intellij.patterns.PlatformPatterns
import com.intellij.util.ProcessingContext

class MyToolWindowFactory : CompletionContributor() {
    init {
        // Intercept BASIC and SMART completions
        extend(CompletionType.BASIC, PlatformPatterns.psiElement(), InterceptProvider())
        extend(CompletionType.SMART, PlatformPatterns.psiElement(), InterceptProvider())
    }
}

// Intercepts all completion items
class InterceptProvider : CompletionProvider<CompletionParameters>() {
    override fun addCompletions(
        parameters: CompletionParameters,
        context: ProcessingContext,
        result: CompletionResultSet
    ) {
        // Map of lookupString → list of items
        val itemsByName = mutableMapOf<String, MutableList<LookupElement>>()

        // Collect all completions from other contributors
        result.runRemainingContributors(parameters) { completion ->
            val item = completion.lookupElement
            itemsByName.computeIfAbsent(item.lookupString) { mutableListOf() }.add(item)
        }

        // For each name, sort so templates come first
        for ((_, items) in itemsByName) {
            val sorted = items.sortedWith(compareByDescending { it.isTemplate() })
            for (item in sorted) {
                result.addElement(item)
            }
        }
    }

    // Helper to check if a lookup element is a live template
    private fun LookupElement.isTemplate(): Boolean {
        val keyField = LookupElement::class.java.getField("TEMPLATE_KEY")
        val key = keyField.get(null) as com.intellij.openapi.util.Key<*>
        return getUserData(key) != null
    }
}