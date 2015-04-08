// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSParserImpl_h
#define CSSParserImpl_h

#include "core/CSSPropertyNames.h"
#include "core/css/CSSProperty.h"
#include "core/css/CSSPropertySourceData.h"
#include "core/css/parser/CSSParserMode.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class StyleRule;
class StyleRuleBase;
class StyleRuleFontFace;
class StyleRuleImport;
class StyleRuleKeyframe;
class StyleRuleKeyframes;
class StyleRuleMedia;
class StyleRuleViewport;
class StyleSheetContents;
class ImmutableStylePropertySet;
class Element;
class MutableStylePropertySet;

class CSSParserImpl {
    STACK_ALLOCATED();
public:
    CSSParserImpl(const CSSParserContext&, const String&, StyleSheetContents* = nullptr);

    enum AllowedRulesType {
        // As per css-syntax, css-cascade and css-namespaces, @charset rules
        // must come first, followed by @import then @namespace.
        // AllowImportRules actually means we allow @import and any rules thay
        // may follow it, i.e. @namespace rules and regular rules.
        // AllowCharsetRules and AllowNamespaceRules behave similarly.
        AllowCharsetRules,
        AllowImportRules,
        AllowNamespaceRules,
        RegularRules,
        KeyframeRules
    };

    static bool parseValue(MutableStylePropertySet*, CSSPropertyID, const String&, bool important, const CSSParserContext&);
    static PassRefPtrWillBeRawPtr<ImmutableStylePropertySet> parseInlineStyleDeclaration(const String&, Element*);
    static bool parseDeclaration(MutableStylePropertySet*, const String&, const CSSParserContext&);
    static PassRefPtrWillBeRawPtr<StyleRuleBase> parseRule(const String&, const CSSParserContext&, AllowedRulesType);
    static void parseStyleSheet(const String&, const CSSParserContext&, StyleSheetContents*);

    static PassOwnPtr<Vector<double>> parseKeyframeKeyList(const String&);

private:
    enum RuleListType {
        TopLevelRuleList,
        RegularRuleList,
        KeyframesRuleList
    };

    WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>> consumeRuleList(CSSParserTokenRange, RuleListType);

    // These two functions update the range they're given and allowed rules
    PassRefPtrWillBeRawPtr<StyleRuleBase> consumeAtRule(CSSParserTokenRange&, AllowedRulesType&);
    PassRefPtrWillBeRawPtr<StyleRuleBase> consumeQualifiedRule(CSSParserTokenRange&, AllowedRulesType&);

    PassRefPtrWillBeRawPtr<StyleRuleImport> consumeImportRule(CSSParserTokenRange prelude);
    void consumeNamespaceRule(CSSParserTokenRange prelude); // This modifies m_styleSheet directly!
    PassRefPtrWillBeRawPtr<StyleRuleMedia> consumeMediaRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleViewport> consumeViewportRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleFontFace> consumeFontFaceRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleKeyframes> consumeKeyframesRule(bool webkitPrefixed, CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleKeyframe> consumeKeyframeStyleRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRule> consumeStyleRule(CSSParserTokenRange prelude, CSSParserTokenRange block);

    // FIXME: We should use a CSSRule::Type here
    void consumeDeclarationList(CSSParserTokenRange, CSSRuleSourceData::Type);
    void consumeDeclaration(CSSParserTokenRange, CSSRuleSourceData::Type);
    void consumeDeclarationValue(CSSParserTokenRange, CSSPropertyID, bool important, CSSRuleSourceData::Type);

    static PassOwnPtr<Vector<double>> consumeKeyframeKeyList(CSSParserTokenRange);

    // FIXME: Can we build StylePropertySets directly?
    // FIXME: Investigate using a smaller inline buffer
    WillBeHeapVector<CSSProperty, 256> m_parsedProperties;
    Vector<CSSParserToken> m_tokens;
    CSSParserContext m_context;

    AtomicString m_defaultNamespace;
    RawPtrWillBeMember<StyleSheetContents> m_styleSheet;
};

} // namespace blink

#endif // CSSParserImpl_h